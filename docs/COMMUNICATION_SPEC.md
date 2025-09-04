# Communication Specification (CaravanOS)

**Status:** Draft (active development)
**Principle:** **One Metric = One Message**

This document defines the message schema, enums, ID registry, property semantics, and basic bus behavior used across CaravanOS.

---

## 1) Metric – Message Schema

A **Metric** transports exactly one value plus metadata.

| Field          | Type                                   | Notes |
|----------------|----------------------------------------|-------|
| `instance_id`  | `uint32`                               | Unique per device instance (e.g., bus address) |
| `metric_id`    | `MetricID` (`uint16`)                  | Concrete measurement identifier (see registry) |
| `datatype`     | `DataType` (`uint8`)                   | **Derived** from `value` (mirror, not authoritative) |
| `value`        | `variant<float, int32_t, bool>`        | Payload value |
| `timestamp_ms` | `uint64`                               | Milliseconds from HAL clock (monotonic) |
| `seq`          | `uint32`                               | Monotonic per instance; reset implies reboot/rejoin |
| `props`        | `map<PropertyKey, variant<uint8_t,int32_t,float>>` | Typed properties (see below) |

### 1.1 DataType Mapping
- `float`  → `DataType::Float`
- `int32`  → `DataType::Int32`
- `bool`   → `DataType::Bool`

There is **no string** payload type by design (embedded-friendly, predictable memory).

### 1.2 Properties

**Key enum:** `PropertyKey` (`uint8`)  
**Value:** `uint8_t | int32_t | float` (no strings)

| Key        | Value type             | Meaning |
|------------|------------------------|---------|
| `Unit`     | `uint8` → `Unit`       | Physical unit of the value |
| `Quality`  | `uint8` → `Quality`    | Data quality: `Good` (0), `Uncertain` (1), `Bad` (2) |
| `Scale`    | `float`                | Calibration: `physical = value * Scale + Offset` |
| `Offset`   | `float`                | Calibration offset |
| `Min`      | `float` or `int32`     | Minimum meaningful physical value (match value type) |
| `Max`      | `float` or `int32`     | Maximum meaningful physical value |
| `SensorId` | `int32`                | Supplier/bus/channel identifier |

**Conventions**
- For `float` metrics, `Min/Max/Scale/Offset` are `float`.
- For `int32` metrics, `Min/Max` are `int32`. `Scale/Offset` are typically unused.
- `bool` metrics usually do not carry `Min/Max`.

---

## 2) Enums

### 2.1 DataType (`uint8`)
- `Float = 1`
- `Int32 = 2`
- `Bool  = 3`

### 2.2 Quality (`uint8`)
- `Good = 0`
- `Uncertain = 1`
- `Bad = 2`

### 2.3 Unit (`uint8`)
- `None = 0`
- `Percent = 1`
- `Degree = 2`
- `Celsius = 3`
- `Volt = 4`
- `Ampere = 5`
- `Watt = 6`

### 2.4 PropertyKey (`uint8`)
- `Unit = 1`
- `Quality = 2`
- `Scale = 4`
- `Offset = 5`
- `Min = 6`
- `Max = 7`
- `SensorId = 8`

---

## 3) MetricID – Single, Append-Only Registry

One global `MetricID : uint16_t` registry. IDs are **never repurposed**; new entries are **append-only**.

Recommended domain ranges (convention, not enforced):
- `0x1000–0x10FF` Water tank
- `0x1100–0x11FF` Gas
- `0x1200–0x12FF` Tilt/IMU
- `0x2000–0x2FFF` Generic (temperature, electrical, …)
- `0xF000–0xF0FF` Health/diagnostics

**Current IDs:**
- `WaterLevel = 0x1001`  — expected `Unit::Percent`, `DataType::Float`
- `GasLevel   = 0x1101`  — expected `Unit::Percent`, `DataType::Float`
- `TiltAngle  = 0x1201`  — expected `Unit::Degree`,  `DataType::Float`
- `Temperature= 0x2001`  — expected `Unit::Celsius`, `DataType::Float`
- `Electrical = 0x2101`  — unit depends on context (`Volt`/`Ampere`/`Watt`), value type typically `Float`
- `Health     = 0xF001`  — `DataType::Int32` health code (`0` = OK, non-zero = fault)

> If a particular metric requires a specific unit, set it via `props[PropertyKey::Unit]`. Consumers must not rely on string names—only enums/IDs.

---

## 4) Bus Semantics

- **Publish/Subscribe**: Subscribers receive `const Metric&`.
- **Thread-safe**: internal locking; safe to publish from within callbacks (re-entrant).
- **Delivery**: by-value copy per subscriber; subscribers must copy if they need to keep data.

---

## 5) Device Policies (Compile-Time)

Devices are typed via a **SpecTag** and may only publish a defined subset of `MetricID`s.

- `SpecPolicy<SpecTag>::allowed` lists the permitted `MetricID`s.
- `DeviceBase<SpecTag>::publish<MetricID>(Metric&&)` does a compile-time `static_assert` using that policy.
- In debug builds, a runtime guard verifies the `metric_id` matches the template parameter.

**Default tags and policies (subject to extension):**
- `WaterTankTag`: `WaterLevel`, `Temperature`, `Health`
- `GasBottleTag`: `GasLevel`, `Health`
- `TiltUnitTag`:  `TiltAngle`, `Health`

**Rules**
- Extend policies append-only.
- Add new metrics by adding new `MetricID` values; do not change existing numeric values or meaning.
