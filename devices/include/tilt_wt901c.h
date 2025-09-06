#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <memory>

#include <core/device_base.hpp>
#include <core/metric.h>
#include <core/enums.h>
#include <core/ids.h>
#include <core/spec_policy.h>
#include "clock.h"
#include "modbus.h"

namespace devices {

class Wt901cDevice : public core::DeviceBase<core::TiltUnitTag> {
public:
  struct Config {
    uint8_t  modbus_addr{1};      // RS485 slave address
    uint16_t start_reg{0};        // erster Holding-Register (wird vom Decoder interpretiert)
    uint16_t reg_count{4};        // Anzahl Register (minimal für Decoder)
    uint32_t poll_interval_ms{50};
  };

  Wt901cDevice(core::MetricBus& bus,
               core::Metric::InstanceId instance_id,
               IModbusClient& modbus,
               Config cfg);

  void tick(uint64_t ts) override;
  bool read_once_and_publish(uint64_t ts);

private:
  core::Metric mk_angle_metric_(float deg, uint64_t ts);
  std::optional<float> decode (const std::vector<uint16_t>& data);

  IModbusClient& modbus_;
  Config         cfg_;
  uint64_t       last_poll_ms_{0};
  uint32_t       seq_{0};
};

} // namespace devices
