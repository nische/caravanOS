# CaravanOS

CaravanOS is a C++17 platform for a smart caravan hub: a single-process system with an event bus, an enum/ID-based payload spec (no magic strings), and a shared codebase for Linux (host) and ESP32 (ESP-IDF).

## Goals

- Monolithic hub (no microservice sprawl)
- Event-bus architecture — **One Metric = One Message**
- Strict enum/ID model (no magic strings)
- Compile-time device policies (devices can only publish their allowed metrics)
- Same sources for host (Linux/tests) and ESP32 (ESP-IDF)
- Solid unit tests (GTest/GMock), devcontainer for a consistent toolchain

## Design Highlights

Metric: value-type message (float/int32/bool) with timestamp, sequence, and typed properties.
MetricBus: thread-safe publish/subscribe; re-entrant callbacks allowed.
MetricID: single, append-only ID space across domains.
Spec Policy: lightweight compile-time enforcement via DeviceBase<SpecTag>::publish<MetricID>(...).

Usage and component creation will be shown in examples/ later
