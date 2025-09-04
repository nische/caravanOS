#pragma once
#include <cstdint>

namespace core {

// Append-only. Nummernräume (nur Konvention für Übersicht):
// 0x1000-0x10FF  WaterTank
// 0x1100-0x11FF  Gas
// 0x1200-0x12FF  Tilt/IMU
// 0x2000-0x2FFF  Generic (Temp, Electrical, ...)
 // 0xFF00-0xFFFF  Health/Diagnostics
enum class MetricID : uint16_t {
  // WaterTank (0x100x)
  WaterLevelPercent   = 0x1001,

  // Gas (0x110x)
  GasLevelPercent     = 0x1101,

  // Tilt/IMU (0x120x)
  TiltAngle    = 0x1201,

  // Generic (0x200x)
  Temperature  = 0x2001,
  Electrical   = 0x2101,  // z. B. Summenleistung/Leitungswert

  // Health (0xF0xx)
  Health       = 0xFF01,
};

} // namespace core
