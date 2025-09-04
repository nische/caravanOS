#include "modbus.h"
#include <random>

class DummyModbus : public IModbusClient {
public:
  bool read_holding(uint8_t, uint16_t, uint16_t n,
                    std::vector<uint16_t>& out, uint32_t) override {
    out.resize(n);
    // Fill with deterministic pseudo-values
    for (uint16_t i = 0; i < n; ++i) out[i] = static_cast<uint16_t>(i * 10u);
    return true;
  }
};

std::unique_ptr<IModbusClient> hal_make_modbus() {
  return std::make_unique<DummyModbus>();
}
