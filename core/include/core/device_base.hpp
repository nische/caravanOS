#pragma once
#include <type_traits>
#include "core/metric_bus.h"
#include "core/ids.h"
#include "core/spec_policy.h" // is_allowed_id, SpecPolicy

namespace core
{

  template <typename SpecTag>
  class DeviceBase
  {
  public:
    using InstanceId = Metric::InstanceId;

    DeviceBase(MetricBus &bus, InstanceId id) : bus_(bus), id_(id) {}
    virtual ~DeviceBase() = default;

    InstanceId instance_id() const noexcept { return id_; }
    MetricBus &bus() noexcept { return bus_; }

  protected:
    // compile-time Enforcement: nur erlaubte MetricIDs
    template <MetricID ID>
    void publish(Metric &&m)
    {
      static_assert(is_allowed_id<SpecTag>(ID),
                    "MetricID not allowed for this device's SpecTag");
#ifndef NDEBUG
      if (m.metric_id() != ID)
        std::abort();
#endif
      bus_.publish(m);
    }

    MetricBus &bus_;
    InstanceId id_;
  };

} // namespace core
