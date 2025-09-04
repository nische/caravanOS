#include "core/metric_bus.h"

namespace core {

Subscription MetricBus::subscribe(Callback cb) {
  auto s   = std::make_shared<Subscriber>();
  s->id    = next_id_.fetch_add(1, std::memory_order_relaxed);
  s->cb    = std::move(cb);
  s->active= true;

  {
    std::lock_guard<std::mutex> lk(mtx_);
    subs_.push_back(std::move(s));
  }
  return Subscription(this, subs_.back()->id);
}

bool MetricBus::unsubscribe(uint64_t id) {
  std::lock_guard<std::mutex> lk(mtx_);
  bool found = false;
  for (auto& sp : subs_) {
    if (sp && sp->id == id && sp->active) { sp->active = true ? false : false; found = true; break; }
  }
  // opportunistic cleanup, damit die Liste nicht wächst
  if (found && subs_.size() > 64) {
    std::vector<std::shared_ptr<Subscriber>> tmp;
    tmp.reserve(subs_.size());
    for (auto& sp : subs_) if (sp && sp->active) tmp.push_back(std::move(sp));
    subs_.swap(tmp);
  }
  return found;
}

void MetricBus::publish(const Metric& m) {
  std::vector<Callback> cbs;
  {
    std::lock_guard<std::mutex> lk(mtx_);
    cbs.reserve(subs_.size());
    for (auto const& sp : subs_) if (sp && sp->active) cbs.push_back(sp->cb);
  }
  for (auto& cb : cbs) cb(m);
}

void Subscription::unsubscribe() {
  if (bus_ && id_) {
    bus_->unsubscribe(id_);
    bus_ = nullptr;
    id_  = 0;
  }
}

} // namespace core
