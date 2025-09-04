#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <type_traits>

#include <core/metric.h>
#include <core/enums.h>
#include <core/ids.h>

using namespace core;

namespace {
// kleine Prop-Helper
inline Metric::PropMap make_props(Unit u, Quality q) {
  Metric::PropMap p;
  p.emplace(PropertyKey::Unit,    static_cast<uint8_t>(u));
  p.emplace(PropertyKey::Quality, static_cast<uint8_t>(q));
  return p;
}
} // namespace

TEST(Metric, Make_InfersDatatype_AndProvidesTypedAccess) {
  // Float
  {
    Metric m = Metric::Make(42, MetricID::WaterLevelPercent, 73.5f, 123456, 1, make_props(Unit::Percent, Quality::Good));
    EXPECT_EQ(m.instance_id(), 42u);
    EXPECT_EQ(m.metric_id(), MetricID::WaterLevelPercent);
    EXPECT_EQ(m.datatype(), DataType::Float);
    ASSERT_NE(m.get_if<float>(), nullptr);
    EXPECT_FLOAT_EQ(*m.get_if<float>(), 73.5f);

    auto r = m.try_get_ref<float>();
    ASSERT_TRUE(r.has_value());
    EXPECT_FLOAT_EQ(r->get(), 73.5f);

    auto v = m.try_get_value<float>();
    ASSERT_TRUE(v.has_value());
    EXPECT_FLOAT_EQ(*v, 73.5f);
  }

  // Int32
  {
    Metric m = Metric::Make(7, MetricID::Health, int32_t{-2}, 99, 4, make_props(Unit::None, Quality::Uncertain));
    EXPECT_EQ(m.datatype(), DataType::Int32);
    ASSERT_NE(m.get_if<int32_t>(), nullptr);
    EXPECT_EQ(*m.get_if<int32_t>(), -2);

    auto r = m.try_get_ref<int32_t>();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->get(), -2);

    auto v = m.try_get_value<int32_t>();
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, -2);
  }

  // Bool
  {
    Metric m = Metric::Make(3, MetricID::Electrical, true, 11, 2, make_props(Unit::Watt, Quality::Good));
    EXPECT_EQ(m.datatype(), DataType::Bool);
    ASSERT_NE(m.get_if<bool>(), nullptr);
    EXPECT_TRUE(*m.get_if<bool>());

    auto r = m.try_get_ref<bool>();
    ASSERT_TRUE(r.has_value());
    EXPECT_TRUE(r->get());

    auto v = m.try_get_value<bool>();
    ASSERT_TRUE(v.has_value());
    EXPECT_TRUE(*v);
  }
}

TEST(Metric, Props_SetGet_TypeSafety) {
  Metric m = Metric::Make(10, MetricID::WaterLevelPercent, 55.0f, 100, 1, {});
  EXPECT_FALSE(m.has_prop(PropertyKey::Unit));
  EXPECT_FALSE(m.has_prop(PropertyKey::Quality));

  m.set_prop(PropertyKey::Unit,    static_cast<uint8_t>(Unit::Percent));
  m.set_prop(PropertyKey::Quality, static_cast<uint8_t>(Quality::Good));
  EXPECT_TRUE(m.has_prop(PropertyKey::Unit));
  EXPECT_TRUE(m.has_prop(PropertyKey::Quality));

  auto u = m.try_get_prop<uint8_t>(PropertyKey::Unit);
  ASSERT_TRUE(u.has_value());
  EXPECT_EQ(*u, static_cast<uint8_t>(Unit::Percent));

  auto q = m.try_get_prop<uint8_t>(PropertyKey::Quality);
  ASSERT_TRUE(q.has_value());
  EXPECT_EQ(*q, static_cast<uint8_t>(Quality::Good));

  // Typ-Mismatch → nullopt
  auto wrong = m.try_get_prop<float>(PropertyKey::Unit);
  EXPECT_FALSE(wrong.has_value());
}

TEST(Metric, Equality_IgnoresPropInsertionOrder) {
  Metric::PropMap p1;
  p1.emplace(PropertyKey::Unit,    static_cast<uint8_t>(Unit::Percent));
  p1.emplace(PropertyKey::Quality, static_cast<uint8_t>(Quality::Good));

  Metric::PropMap p2;
  p2.emplace(PropertyKey::Quality, static_cast<uint8_t>(Quality::Good)); // umgekehrte Reihenfolge
  p2.emplace(PropertyKey::Unit,    static_cast<uint8_t>(Unit::Percent));

  Metric a = Metric::Make(1, MetricID::WaterLevelPercent, 12.5f, 1000, 10, p1);
  Metric b = Metric::Make(1, MetricID::WaterLevelPercent, 12.5f, 1000, 10, p2);

  EXPECT_TRUE(a == b);

  // Unterschiedliche Props → ungleich
  b.set_prop(PropertyKey::Min, 0.0f);
  EXPECT_FALSE(a == b);
}

TEST(Metric, CopyAndMoveSemantics) {
  Metric::PropMap p;
  p.emplace(PropertyKey::Unit,    static_cast<uint8_t>(Unit::Percent));
  p.emplace(PropertyKey::Quality, static_cast<uint8_t>(Quality::Good));

  Metric a = Metric::Make(9, MetricID::WaterLevelPercent, 33.0f, 777, 5, p);
  Metric b = a;                // copy
  EXPECT_TRUE(a == b);

  Metric c = std::move(b);     // move
  EXPECT_TRUE(a == c);         // moved-to behält Inhalte
  // moved-from b ist in undefiniertem, aber validem Zustand → nicht verwenden
}

TEST(Metric, TimestampsAndSeqArePreserved) {
  Metric m = Metric::Make(2, MetricID::Health, int32_t{0}, 424242, 99, make_props(Unit::None, Quality::Bad));
  EXPECT_EQ(m.timestamp_ms(), 424242u);
  EXPECT_EQ(m.seq(), 99u);
}
