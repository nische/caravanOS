#pragma once
#include <cstdint>
#include <memory>
#include <vector>
struct IModbusClient {
  virtual ~IModbusClient() = default;
  virtual bool read_holding(uint8_t addr, uint16_t reg, uint16_t n,
                            std::vector<uint16_t>& out, uint32_t timeout_ms) = 0;
};

// Provide a factory function
std::unique_ptr<IModbusClient> hal_make_modbus();