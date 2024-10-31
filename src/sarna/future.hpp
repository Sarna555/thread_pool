#ifndef __FUTURE_H__
#define __FUTURE_H__
#include <future>

#include "thread_pool.hpp"

namespace sarna
{

template<typename R>
class Future
{
public:

    template <class F, class R = std::invoke_result_t<F>>
    Future(F &&f, tasks *pool = default_pool() )
    {
        _f = _pool->queue(std::forward<F>(f));
    }
private:
    tasks* _pool = nullptr;
    std::future<R> _f;
    bool _valid = true;
};

}  // namespace sarna

#endif
