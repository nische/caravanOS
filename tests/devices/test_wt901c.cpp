#include <gtest/gtest.h>
#include <core/metric_bus.h>
#include <core/metric.h>
#include <core/enums.h>
#include <core/ids.h>

#include "tilt_wt901c.h"
#include "modbus.h"

using namespace core;
using namespace devices;


class FakeModbus : public IModbusClient {
public:
  bool ok{true};
  std::vector<uint16_t> next_regs;
  mutable uint8_t last_addr{0};
  mutable uint16_t last_reg{0}, last_n{0};
  bool read_holding(uint8_t addr, uint16_t reg, uint16_t n,
                    std::vector<uint16_t>& out, uint32_t) override {
    last_addr = addr; last_reg = reg; last_n = n;
    if (!ok) return false;
    out = next_regs;
    return true;
  }
};

TEST(WT901C, Publishes_TiltAngle_OnSuccessfulReadAndDecode) {
  MetricBus bus;
  FakeModbus mb;

  Wt901cDevice::Config cfg;
  cfg.modbus_addr = 7;
  cfg.start_reg   = 0x0030;
  cfg.reg_count   = 2;
  cfg.poll_interval_ms = 50;

  Wt901cDevice dev(bus, /*instance*/ 0x1201u, mb, cfg);

  // Subscription auf TiltAngle
  std::vector<Metric> got;
  auto sub = bus.subscribe([&](const Metric& m){
    if (m.metric_id() == MetricID::TiltAngle) got.push_back(m);
  });

  // Fake-Modbus liefert 1234 -> 12.34°
  mb.next_regs = { 1234, 0 };

  // direkt aufrufen (nicht über tick)
  ASSERT_TRUE(dev.read_once_and_publish(1500));
  ASSERT_EQ(got.size(), 1u);

  const auto& m = got[0];
  EXPECT_EQ(m.instance_id(), 0x1201u);
  EXPECT_EQ(m.metric_id(), MetricID::TiltAngle);
  EXPECT_EQ(m.datatype(), DataType::Float);
  ASSERT_NE(m.get_if<float>(), nullptr);
  EXPECT_FLOAT_EQ(*m.get_if<float>(), 12.34f);

  // Props prüfen
  auto unit_it = m.props().find(PropertyKey::Unit);
  ASSERT_TRUE(unit_it != m.props().end());
  EXPECT_EQ(std::get<uint8_t>(unit_it->second), static_cast<uint8_t>(Unit::Degree));

  auto q_it = m.props().find(PropertyKey::Quality);
  ASSERT_TRUE(q_it != m.props().end());
  EXPECT_EQ(std::get<uint8_t>(q_it->second), static_cast<uint8_t>(Quality::Good));
}

// TEST(WT901C, TickHonorsPollInterval) {
//   MetricBus bus;
//   FakeClock clk;
//   FakeModbus mb;

//   auto decoder = [](const std::vector<uint16_t>& regs)->std::optional<float>{
//     if (regs.empty()) return std::nullopt;
//     return static_cast<float>(regs[0]) * 0.1f;
//   };

//   Wt901cDevice dev(bus, 1u, clk, mb, {}, decoder);

//   size_t count = 0;
//   auto sub = bus.subscribe([&](const Metric& m){
//     if (m.metric_id() == MetricID::TiltAngle) ++count;
//   });

//   mb.next_regs = {100};

//   // 1. Tick → publish
//   dev.tick();
//   EXPECT_EQ(count, 1u);

//   // 2. Tick zu früh → kein publish
//   dev.tick();
//   EXPECT_EQ(count, 1u);

//   // Zeit vorspulen → publish wieder
//   clk.now += 100;
//   dev.tick();
//   EXPECT_EQ(count, 2u);
// }

// TEST(WT901C, ReadErrorOrDecodeFail_DoesNotPublish) {
//   MetricBus bus;
//   FakeClock clk;
//   FakeModbus mb;

//   Wt901cDevice dev(bus, 1u, clk, mb, {}, [](auto&)->std::optional<float>{ return std::nullopt; });

//   size_t count = 0;
//   auto sub = bus.subscribe([&](const Metric& m){
//     if (m.metric_id() == MetricID::TiltAngle) ++count;
//   });

//   // Decoder liefert nullopt → kein Publish
//   mb.next_regs = { 1, 2, 3 };
//   EXPECT_FALSE(dev.read_once_and_publish());
//   EXPECT_EQ(count, 0u);

//   // Modbus-Fehler → kein Publish
//   mb.ok = false;
//   EXPECT_FALSE(dev.read_once_and_publish());
//   EXPECT_EQ(count, 0u);
// }
