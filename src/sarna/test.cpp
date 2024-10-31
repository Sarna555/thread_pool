#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

#include "thread_pool.hpp"

using namespace std::chrono_literals;

// uint32_t factorial(uint32_t number)
// {
//     return number <= 1 ? number : factorial(number - 1) * number;
// }

// TEST_CASE("Factorials are computed", "[factorial]")
// {
//     REQUIRE(factorial(1) == 1);
//     REQUIRE(factorial(2) == 2);
//     REQUIRE(factorial(3) == 6);
//     REQUIRE(factorial(10) == 3'628'800);
// }

TEST_CASE("Create thread pool", "[thread_pool]")
{
    sarna::tasks pool;
    pool.start();

    auto f = pool.queue([]() -> int { return 2; });
    auto status = f.wait_for(1s);

    REQUIRE(status == std::future_status::ready);
    REQUIRE(f.get() == 2);
}

TEST_CASE("Two concurrent taska", "[thread_pool]")
{
    sarna::tasks pool;
    pool.start(2);

    auto f1 = pool.queue(
        []() -> int
        {
            std::this_thread::sleep_for(980ms);
            return 2;
        });
    auto f2 = pool.queue([]() -> int { return 7; });
    auto status2 = f2.wait_for(100ms);
    auto status1 = f1.wait_for(1s);

    REQUIRE(status1 == std::future_status::ready);
    REQUIRE(status2 == std::future_status::ready);
    REQUIRE(f1.get() == 2);
    REQUIRE(f2.get() == 7);
}
