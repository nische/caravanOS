#include "core/metric.h"

namespace core {

static_assert(std::is_same_v<Metric::Value, std::variant<float,int32_t,bool>>,
              "Metric::Value must match DataType enum");

DataType Metric::InferDatatype(const Value& v) noexcept {
  switch (v.index()) {
    case 0: return DataType::Float;
    case 1: return DataType::Int32;
    case 2: return DataType::Bool;
    default: return DataType::Int32;
  }
}

Metric Metric::Make(InstanceId id, MetricID metric_id, Value v,
                    uint64_t ts_ms, uint32_t seq, PropMap props) {
  Metric m;
  m.instance_id_ = id;
  m.metric_id_   = metric_id;
  m.datatype_    = InferDatatype(v);
  m.value_       = std::move(v);
  m.timestamp_ms_= ts_ms;
  m.seq_         = seq;
  m.props_       = std::move(props);
  return m;
}

void Metric::set_prop(PropertyKey k, PropValue v) {
  props_.emplace(k, std::move(v));
}

bool operator==(const Metric& a, const Metric& b) {
  return a.instance_id_ == b.instance_id_
      && a.metric_id_    == b.metric_id_
      && a.datatype_     == b.datatype_
      && a.value_        == b.value_
      && a.timestamp_ms_ == b.timestamp_ms_
      && a.seq_          == b.seq_
      && a.props_        == b.props_;
}

} // namespace core
