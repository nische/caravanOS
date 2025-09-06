#include "tilt_wt901c.h"
#include <utility>

using namespace core;

namespace devices {

Wt901cDevice::Wt901cDevice(MetricBus& bus,
                           Metric::InstanceId instance_id,
                           IModbusClient& modbus,
                           Config cfg)
: DeviceBase<TiltUnitTag>(bus, instance_id)
, modbus_(modbus)
, cfg_(cfg)
{}

void Wt901cDevice::tick(uint64_t ts) {
  if (ts - last_poll_ms_ < cfg_.poll_interval_ms) return;
  last_poll_ms_ = ts;
  (void)read_once_and_publish(ts);
}

bool Wt901cDevice::read_once_and_publish(uint64_t ts) {
  std::vector<uint16_t> regs;
  if (!modbus_.read_holding(cfg_.modbus_addr, cfg_.start_reg, cfg_.reg_count, regs, /*timeout_ms*/20)) {
    return false;
  }
  auto angle_opt = decode(regs);
  if (!angle_opt.has_value()) return false;

  auto m = mk_angle_metric_(*angle_opt, ts);

  publish<MetricID::TiltAngle>(std::move(m));
  return true;
}

std::optional<float> Wt901cDevice::decode(const std::vector<uint16_t>& data) {
  if (data.size() < 4) return std::nullopt;
  // Beispielhafte Dekodierung: Wir nehmen an, dass der Neigungswinkel im 2. Register gespeichert ist
  float angle = static_cast<float>(data[1]) * 0.1f; // Umrechnung in Grad
  return angle;
}

Metric Wt901cDevice::mk_angle_metric_(float deg, uint64_t ts) {
  Metric::PropMap p;
  p.emplace(PropertyKey::Unit,    static_cast<uint8_t>(Unit::Degree));
  p.emplace(PropertyKey::Quality, static_cast<uint8_t>(Quality::Good));
  return Metric::Make(
      /*instance*/ id_,
      /*metric*/   MetricID::TiltAngle,
      /*value*/    deg,
      /*ts*/       ts,
      /*seq*/      ++seq_,
      /*props*/    std::move(p)
  );
}

} // namespace devices
