#pragma once
#include <cstdint>
#include <memory>

struct IClock {
  virtual ~IClock() = default;
  virtual uint64_t millis64() = 0;
};

// Factory function specification for IClock using unique_ptr
std::unique_ptr<IClock> hal_make_clock();