#pragma once
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <variant>

#include "core/enums.h"
#include "core/ids.h"
#include "core/enum_hash.hpp"

namespace core {

class Metric {
public:
  using InstanceId = uint32_t;

  using Value     = std::variant<float, int32_t, bool>;
  using PropValue = std::variant<uint8_t, int32_t, float>;
  using PropMap   = std::unordered_map<PropertyKey, PropValue, detail::enum_hash<PropertyKey>>;

  // Fabrik
  static Metric Make(InstanceId id, MetricID metric_id, Value v,
                     uint64_t ts_ms, uint32_t seq, PropMap props = {});

  // triviale Getter (noexcept)
  InstanceId instance_id() const noexcept { return instance_id_; }
  MetricID   metric_id()   const noexcept { return metric_id_; }
  DataType   datatype()    const noexcept { return datatype_; }
  uint64_t   timestamp_ms()const noexcept { return timestamp_ms_; }
  uint32_t   seq()         const noexcept { return seq_; }
  const PropMap& props()   const noexcept { return props_; }

  // Value-Zugriff
  template<typename T>
  const T* get_if() const noexcept { return std::get_if<T>(&value_); }

  // ergonomische Alternativen
  template<typename T>
  std::optional<std::reference_wrapper<const T>> try_get_ref() const noexcept {
    if (auto p = std::get_if<T>(&value_)) return std::cref(*p);
    return std::nullopt;
  }
  template<typename T>
  std::optional<T> try_get_value() const noexcept {
    if (auto p = std::get_if<T>(&value_)) return *p;
    return std::nullopt;
  }

  // Props
  void set_prop(PropertyKey k, PropValue v);
  template<typename T>
  std::optional<T> try_get_prop(PropertyKey k) const {
    auto it = props_.find(k);
    if (it == props_.end()) return std::nullopt;
    if (auto p = std::get_if<T>(&it->second)) return *p;
    return std::nullopt;
  }
  bool has_prop(PropertyKey k) const noexcept { return props_.find(k) != props_.end(); }

  // Vergleich (für Tests)
  friend bool operator==(const Metric& a, const Metric& b);

private:
  static DataType InferDatatype(const Value& v) noexcept;

  // nur über Make()
  Metric() = default;

  InstanceId instance_id_{0};
  MetricID   metric_id_{MetricID::Health};
  DataType   datatype_{DataType::Int32};
  Value      value_{};
  uint64_t   timestamp_ms_{0};
  uint32_t   seq_{0};
  PropMap    props_{};
};

} // namespace core
