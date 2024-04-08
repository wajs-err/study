#include <twist/test/with/wheels/stress.hpp>

#include <twist/test/repeat.hpp>
#include <twist/test/random.hpp>

#include <exe/executors/thread_pool.hpp>
#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/sync/wait_group.hpp>
#include <exe/threads/wait_group.hpp>

#include <fmt/core.h>

#include <array>
#include <atomic>
#include <chrono>

using namespace exe;
using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

void StressTest() {
  executors::ThreadPool scheduler{/*threads=*/4};
  scheduler.Start();

  twist::test::Repeat repeat;

  while (repeat()) {
    size_t workers = twist::test::Random(1, 4);
    size_t waiters = twist::test::Random(1, 4);

    threads::WaitGroup iter;
    iter.Add(waiters);

    std::array<bool, 5> work;
    work.fill(false);

    fibers::WaitGroup wg;
    wg.Add(workers);

    // Waiters

    for (size_t i = 0; i < waiters; ++i) {
      fibers::Go(scheduler, [&] {
        wg.Wait();
        for (size_t j = 0; j < workers; ++j) {
          ASSERT_TRUE(work[j]);
        }
        iter.Done();
      });
    }

    // Workers

    for (size_t j = 0; j < workers; ++j) {
      fibers::Go(scheduler, [&, j] {
        work[j] = true;
        wg.Done();
      });
    }

    iter.Wait();
  }

  fmt::println("Iterations: {}", repeat.IterCount());

  scheduler.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(WaitGroup) {
  TWIST_TEST(Stress, 5s) {
    StressTest();
  }
}

RUN_ALL_TESTS()
