#include "clock.h"
#include <chrono>

class PosixClock : public IClock {
public:
  uint64_t millis64() override {
    using namespace std::chrono;
    static const auto t0 = steady_clock::now();
    return duration_cast<milliseconds>(steady_clock::now() - t0).count();
  }
};

// Provide a factory function (optional)
std::unique_ptr<IClock> hal_make_clock() {
  return std::make_unique<PosixClock>();
}
