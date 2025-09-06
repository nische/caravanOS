#include <gtest/gtest.h>
#include <vector>

#include <core/device_base.hpp>
#include <core/metric_bus.h>
#include <core/metric.h>
#include <core/enums.h>
#include <core/ids.h>
#include <core/spec_policy.h>

using namespace core;

// ---- Helpers: gültige Metrics bauen (Public API) ----
namespace {
inline Metric mk_waterlevel(uint32_t inst, float pct, uint64_t ts, uint32_t seq, Quality q = Quality::Good) {
  Metric::PropMap p;
  p.emplace(PropertyKey::Unit,    static_cast<uint8_t>(Unit::Percent));
  p.emplace(PropertyKey::Quality, static_cast<uint8_t>(q));
  return Metric::Make(inst, MetricID::WaterLevelPercent, pct, ts, seq, std::move(p));
}
inline Metric mk_health(uint32_t inst, int32_t code, uint64_t ts, uint32_t seq, Quality q = Quality::Good) {
  Metric::PropMap p;
  p.emplace(PropertyKey::Unit,    static_cast<uint8_t>(Unit::None));
  p.emplace(PropertyKey::Quality, static_cast<uint8_t>(q));
  return Metric::Make(inst, MetricID::Health, code, ts, seq, std::move(p));
}
inline Metric mk_gaslevel(uint32_t inst, float pct, uint64_t ts, uint32_t seq) {
  Metric::PropMap p;
  p.emplace(PropertyKey::Unit,    static_cast<uint8_t>(Unit::Percent));
  p.emplace(PropertyKey::Quality, static_cast<uint8_t>(Quality::Good));
  return Metric::Make(inst, MetricID::GasLevelPercent, pct, ts, seq, std::move(p));
}
inline Metric mk_tilt(uint32_t inst, float deg, uint64_t ts, uint32_t seq) {
  Metric::PropMap p;
  p.emplace(PropertyKey::Unit,    static_cast<uint8_t>(Unit::Degree));
  p.emplace(PropertyKey::Quality, static_cast<uint8_t>(Quality::Good));
  return Metric::Make(inst, MetricID::TiltAngle, deg, ts, seq, std::move(p));
}
} // namespace

// ---- Fake-Devices (Policy-enforced) ----
class WaterTankDevice : public DeviceBase<WaterTankTag> {
public:
  using DeviceBase::DeviceBase;
  void tick(uint64_t ts) {
    publish<MetricID::WaterLevelPercent>( mk_waterlevel(id_, level_pct, ts, ++seq) );
    publish<MetricID::Health>(     mk_health(id_, 0,         ts, ++seq) );
  }
  uint32_t seq{0};
  float    level_pct{0.0f};
};

class GasBottleDevice : public DeviceBase<GasBottleTag> {
public:
  using DeviceBase::DeviceBase;
  void tick(uint64_t ts) {
    publish<MetricID::GasLevelPercent>( mk_gaslevel(id_, gas_pct, ts, ++seq) );
    publish<MetricID::Health>(   mk_health(id_, 0,       ts, ++seq) );
  }
  uint32_t seq{0};
  float    gas_pct{0.0f};
};

class TiltUnitDevice : public DeviceBase<TiltUnitTag> {
public:
  using DeviceBase::DeviceBase;
  void tick(uint64_t ts) {
    publish<MetricID::TiltAngle>( mk_tilt(id_, deg, ts, ++seq) );
    publish<MetricID::Health>(    mk_health(id_, 0,   ts, ++seq) );
  }
  uint32_t seq{0};
  float deg{0.0f};
};

// ---- Tests (Blackbox) ----
TEST(DeviceBase, WaterTank_PublishesAllowedMetrics) {
  MetricBus bus;
  std::vector<MetricID> seen;
  auto sub = bus.subscribe([&](const Metric& m){ seen.push_back(m.metric_id()); });

  WaterTankDevice dev(bus, /*instance*/100);
  dev.level_pct = 67.0f;
  dev.tick(/*ts*/1000);

  ASSERT_EQ(seen.size(), 2u);
  EXPECT_EQ(seen[0], MetricID::WaterLevelPercent);
  EXPECT_EQ(seen[1], MetricID::Health);

  sub.unsubscribe();
}

TEST(DeviceBase, GasBottle_PublishesAllowedMetrics) {
  MetricBus bus;
  std::vector<MetricID> seen;
  auto sub = bus.subscribe([&](const Metric& m){ seen.push_back(m.metric_id()); });

  GasBottleDevice dev(bus, 200);
  dev.gas_pct = 42.0f;
  dev.tick(2000);

  ASSERT_EQ(seen.size(), 2u);
  EXPECT_EQ(seen[0], MetricID::GasLevelPercent);
  EXPECT_EQ(seen[1], MetricID::Health);

  sub.unsubscribe();
}

TEST(DeviceBase, TiltUnit_PublishesAllowedMetrics) {
  MetricBus bus;
  std::vector<MetricID> seen;
  auto sub = bus.subscribe([&](const Metric& m){ seen.push_back(m.metric_id()); });

  TiltUnitDevice dev(bus, 300);
  dev.deg = -12.5f;
  dev.tick(3000);

  ASSERT_EQ(seen.size(), 2u);
  EXPECT_EQ(seen[0], MetricID::TiltAngle);
  EXPECT_EQ(seen[1], MetricID::Health);

  sub.unsubscribe();
}
