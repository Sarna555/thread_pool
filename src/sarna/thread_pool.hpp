#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <utility>

namespace sarna
{

struct tasks
{
    // queue( lambda ) will enqueue the lambda into the tasks for the threads
    // to use.  A future of the type the lambda returns is given to let you get
    // the result out.
    template <class F, class R = std::invoke_result_t<F>>
    std::future<R> queue(F &&f)
    {
        // wrap the function object into a packaged task, splitting
        // execution from the return value:
        std::packaged_task<R()> p(std::forward<F>(f));

        auto r = p.get_future();  // get the return value before we hand off the task
        {
            auto l = std::unique_lock<std::mutex>(_m);
            _work.emplace_back(std::move(p));  // store the task<R()> as a task<void()>
        }
        _v.notify_one();  // wake a thread to work on the task

        return r;  // return the future result of the task
    }

    // start N threads in the thread pool.
    void start(std::size_t N = 1)
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            // each thread is a std::async running this->thread_task():
            _finished.push_back(std::async(std::launch::async, [this] { thread_task(); }));
        }
    }

    // abort() cancels all non-started tasks, and tells every working thread
    // stop running, and waits for them to finish up.
    void abort()
    {
        cancel_pending();
        finish();
    }

    // cancel_pending() merely cancels all non-started tasks:
    void cancel_pending()
    {
        auto l = std::unique_lock<std::mutex>(_m);
        _work.clear();
    }

    // finish enques a "stop the thread" message for every thread, then waits for
    // them:
    void finish()
    {
        {
            auto l = std::unique_lock<std::mutex>(_m);
            for (auto &&unused : _finished)
            {
                _work.push_back({});
            }
        }
        _v.notify_all();
        _finished.clear();
    }

    ~tasks() { finish(); }

private:
    // the work that a worker thread does:
    void thread_task()
    {
        while (true)
        {
            // pop a task off the queue:
            std::move_only_function<void()> f;
            // std::packaged_task<void()> f;
            {
                // usual thread-safe queue code:
                auto l = std::unique_lock<std::mutex>(_m);
                if (_work.empty())
                {
                    _v.wait(l, [&] { return !_work.empty(); });
                }
                f = std::move(_work.front());
                _work.pop_front();
            }
            // if the task is invalid, it means we are asked to abort:
            // if (!f.valid())
            if (!f)
            {
                return;
            }
            // otherwise, run the task:
            f();
        }
    }

    // the mutex, condition variable and deque form a single
    // thread-safe triggered queue of tasks:
    std::mutex _m;
    std::condition_variable _v;
    // note that a packaged_task<void> can store a packaged_task<R>:
    // std::deque<std::packaged_task<void()>> _work;
    std::deque<std::move_only_function<void()>> _work;

    // this holds futures representing the worker threads being done:
    std::vector<std::future<void>> _finished;
};

static tasks global_pool;

static auto default_pool() -> tasks*
{
    return &global_pool;
}

}  // namespace sarna

#endif
