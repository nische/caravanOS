#pragma once
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "core/metric.h"

namespace core {

class Subscription;

class MetricBus {
public:
  using Callback = std::function<void(const Metric&)>;

  MetricBus() = default;
  ~MetricBus() = default;

  // Anmelden; Rückgabe: RAII-Subscription (destructor -> unsubscribe)
  Subscription subscribe(Callback cb);
  bool unsubscribe(uint64_t id);

  // Verteilt by-value an alle aktiven Subscriber
  void publish(const Metric& m);

private:
  struct Subscriber {
    uint64_t id;
    Callback cb;
    bool     active;
  };

  std::mutex mtx_;
  std::vector<std::shared_ptr<Subscriber>> subs_;
  std::atomic<uint64_t> next_id_{1};

  friend class Subscription;
};

class Subscription {
public:
  Subscription() = default;
  ~Subscription() { unsubscribe(); }

  Subscription(Subscription&& o) noexcept { move_from(std::move(o)); }
  Subscription& operator=(Subscription&& o) noexcept {
    if (this != &o) { unsubscribe(); move_from(std::move(o)); }
    return *this;
  }

  Subscription(const Subscription&)            = delete;
  Subscription& operator=(const Subscription&) = delete;

  void unsubscribe();

private:
  Subscription(MetricBus* bus, uint64_t id) : bus_(bus), id_(id) {}
  void move_from(Subscription&& o) noexcept {
    bus_ = o.bus_; id_ = o.id_;
    o.bus_ = nullptr; o.id_ = 0;
  }

  MetricBus* bus_{nullptr};
  uint64_t   id_{0};

  friend class MetricBus;
};

} // namespace core
