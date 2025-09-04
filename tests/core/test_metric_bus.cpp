#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>

#include <core/metric_bus.h>
#include <core/metric.h>
#include <core/enums.h>
#include <core/ids.h>

using namespace core;

namespace
{
    inline Metric mk_health(uint32_t seq)
    {
        Metric::PropMap p;
        p.emplace(PropertyKey::Unit, static_cast<uint8_t>(Unit::None));
        p.emplace(PropertyKey::Quality, static_cast<uint8_t>(Quality::Good));
        return Metric::Make(1, MetricID::Health, int32_t{0}, /*ts*/ 100, seq, p);
    }
} // namespace

class MetricBusFixture : public ::testing::Test
{
protected:
    MetricBus bus_;
};

TEST_F(MetricBusFixture, Subscribe_Publish_Unsubscribe)
{
    int count = 0;
    auto sub = bus_.subscribe([&](const Metric &)
                              { ++count; });
    bus_.publish(mk_health(1));
    EXPECT_EQ(count, 1);

    sub.unsubscribe();
    bus_.publish(mk_health(2));
    EXPECT_EQ(count, 1);

    // idempotent
    sub.unsubscribe();
    EXPECT_EQ(count, 1);
}

TEST_F(MetricBusFixture, RAII_UnsubscribeOnDestructor)
{
    int count = 0;
    {
        auto sub = bus_.subscribe([&](const Metric &)
                                  { ++count; });
        bus_.publish(mk_health(1));
        EXPECT_EQ(count, 1);
        // sub geht hier aus dem Scope → unsubscribe()
    }
    bus_.publish(mk_health(2));
    EXPECT_EQ(count, 1); // kein weiterer Empfang
}

TEST_F(MetricBusFixture, MultipleSubscribers_OrderAndDelivery)
{
    std::vector<int> order;
    order.reserve(2);

    auto sa = bus_.subscribe([&](const Metric &)
                             { order.push_back(0); });
    auto sb = bus_.subscribe([&](const Metric &)
                             { order.push_back(1); });

    bus_.publish(mk_health(1));

    ASSERT_EQ(order.size(), 2u);
    // Erwartung: Reihenfolge der Registrierung
    EXPECT_EQ(order[0], 0);
    EXPECT_EQ(order[1], 1);

    sa.unsubscribe();
    sb.unsubscribe();
}

TEST_F(MetricBusFixture, Reentrancy_SafePublishWithinCallback)
{
    GTEST_FAIL() << "Not implemented";
    //TODO: Hier ist ein übler crash mal gucken. liegt aber scheinbar eher am test!
    int outer = 0, inner = 0;
    auto sub_inner = bus_.subscribe([&](const Metric &)
                                    { ++inner; });
    auto sub_outer = bus_.subscribe([&](const Metric & /*m*/)
                                    {
    ++outer;
    // reentrant publish: frische, triviale Metric
    bus_.publish(mk_health(2)); });

    bus_.publish(mk_health(1));

    EXPECT_EQ(outer, 1);
    // inner empfängt sowohl ursprüngliche als auch reentrant publizierte Metric
    EXPECT_EQ(inner, 2);

    sub_outer.unsubscribe();
    sub_inner.unsubscribe();
}

TEST_F(MetricBusFixture, Concurrency_Smoke)
{
    std::atomic<int> recv{0};
    std::atomic<bool> run{true};

    auto sub = bus_.subscribe([&](const Metric &)
                              { recv.fetch_add(1, std::memory_order_relaxed); });

    std::thread pub([&]
                    {
    for (int i=0; i<2000; ++i) bus_.publish(mk_health(static_cast<uint32_t>(i+1)));
    run.store(false, std::memory_order_release); });

    std::thread churn([&]
                      {
    while (run.load(std::memory_order_acquire)) {
      auto t = bus_.subscribe([&](const Metric&){ /* no-op */ });
      bus_.unsubscribe(0xFFFFFFFFFFFFFFFFull); // nicht existent → sollte false sein
      t.unsubscribe();
    } });

    pub.join();
    churn.join();
    sub.unsubscribe();

    ASSERT_GE(recv.load(), 1);
}

TEST_F(MetricBusFixture, UnsubscribeNonexistentReturnsFalse)
{
    // nie vergebene ID → false
    EXPECT_FALSE(bus_.unsubscribe(0));
    EXPECT_FALSE(bus_.unsubscribe(999'999'999ull));
}
