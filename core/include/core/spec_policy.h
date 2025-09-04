#pragma once
#include <array>
#include "core/ids.h"

namespace core { // DeviceBase erwartet die Symbole im core-NS

// ---- Spec-Tags (Marker) ----
struct WaterTankTag {};
struct GasBottleTag {};
struct TiltUnitTag  {}; // Beispiel für IMU / Neigungssensor

// ---- Primary Template (absichtlich nicht implementiert) ----
template <typename SpecTag>
struct SpecPolicy;

// ---- Spezialisierungen (leichtgewichtig, append-only) ----

// WaterTank darf: Wasserfüllstand, Temperatur (optional), Health
template <>
struct SpecPolicy<WaterTankTag> {
  static constexpr std::array<MetricID, 3> allowed{
    core::MetricID::WaterLevelPercent,
    core::MetricID::Temperature,
    core::MetricID::Health
  };
};

// GasBottle darf: Gasfüllstand, Health
template <>
struct SpecPolicy<GasBottleTag> {
  static constexpr std::array<MetricID, 2> allowed{
    core::MetricID::GasLevelPercent,
    core::MetricID::Health
  };
};

// TiltUnit darf: Neigungswinkel, Health
template <>
struct SpecPolicy<TiltUnitTag> {
  static constexpr std::array<MetricID, 2> allowed{
    MetricID::TiltAngle,
    MetricID::Health
  };
};

// ---- Helper: compile-time Abfrage ----
template <typename SpecTag>
constexpr bool is_allowed_id(MetricID id) {
  for (auto a : SpecPolicy<SpecTag>::allowed) if (a == id) return true;
  return false;
}

} // namespace core
