#pragma once
#include <cstdint>

namespace core {

enum class DataType : uint8_t {
    Float  = 1,
    Int32  = 2,
    Bool   = 3
};

enum class Quality : uint8_t {
    Good      = 0,
    Uncertain = 1,
    Bad      = 2
};

enum class Unit : uint8_t {
    None     = 0,
    Percent  = 1,
    Degree   = 2,
    Celsius  = 3,
    Volt     = 4,
    Ampere   = 5,
    Watt     = 6,
    
};

enum class PropertyKey : uint8_t {
    Unit     = 1,
    Quality  = 2,
    Scale    = 4,
    Offset   = 5,
    Min      = 6,
    Max      = 7,
    SensorId = 8
};

} // namespace core
