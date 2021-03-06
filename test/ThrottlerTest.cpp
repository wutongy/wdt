#include <wdt/Throttler.h>
#include <wdt/test/TestCommon.h>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <chrono>

namespace facebook {
namespace wdt {

void testThrottling(std::shared_ptr<Throttler> throttler, int expectedRate) {
  int64_t numTransferred = 0;
  auto startTime = Clock::now();
  while (durationSeconds(Clock::now() - startTime) <= 2) {
    throttler->limit(1000);
    numTransferred += 1000;
  }
  auto endTime = Clock::now();
  double durationSecs = durationSeconds(endTime - startTime);
  double throughputMBps = numTransferred / durationSecs / kMbToB;
  EXPECT_NEAR(throughputMBps, expectedRate, 1);
}

TEST(ThrottlerTest, RATE_CHANGE) {
  WdtOptions options;
  options.avg_mbytes_per_sec = 50;
  std::shared_ptr<Throttler> throttler = Throttler::makeThrottler(options);
  throttler->startTransfer();

  testThrottling(throttler, 50);

  // test rate decrease
  options.avg_mbytes_per_sec = 30;
  throttler->setThrottlerRates(options);
  testThrottling(throttler, 30);

  // test rate increase
  options.avg_mbytes_per_sec = 70;
  throttler->setThrottlerRates(options);
  testThrottling(throttler, 70);

  throttler->endTransfer();
}
}
}
