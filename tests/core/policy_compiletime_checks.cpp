// Nur Compile-Time-Checks via static_assert. Kein GTest hier.
#include <core/spec_policy.h>
#include <core/ids.h>

using namespace core;

// WaterTankTag
static_assert(is_allowed_id<WaterTankTag>(MetricID::WaterLevelPercent),  "WaterTank should allow WaterLevel");
static_assert(is_allowed_id<WaterTankTag>(MetricID::Temperature), "WaterTank should allow Temperature");
static_assert(is_allowed_id<WaterTankTag>(MetricID::Health),      "WaterTank should allow Health");
static_assert(!is_allowed_id<WaterTankTag>(MetricID::GasLevelPercent),   "WaterTank must NOT allow GasLevel");
static_assert(!is_allowed_id<WaterTankTag>(MetricID::TiltAngle),  "WaterTank must NOT allow TiltAngle");

// GasBottleTag
static_assert(is_allowed_id<GasBottleTag>(MetricID::GasLevelPercent),    "GasBottle should allow GasLevel");
static_assert(is_allowed_id<GasBottleTag>(MetricID::Health),      "GasBottle should allow Health");
static_assert(!is_allowed_id<GasBottleTag>(MetricID::WaterLevelPercent), "GasBottle must NOT allow WaterLevel");

// TiltUnitTag
static_assert(is_allowed_id<TiltUnitTag>(MetricID::TiltAngle),    "TiltUnit should allow TiltAngle");
static_assert(is_allowed_id<TiltUnitTag>(MetricID::Health),       "TiltUnit should allow Health");
static_assert(!is_allowed_id<TiltUnitTag>(MetricID::GasLevelPercent),    "TiltUnit must NOT allow GasLevel");
