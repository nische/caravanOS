add_executable(core_tests
  test_metric.cpp
  test_metric_bus.cpp
  test_device_base.cpp
  policy_compiletime_checks.cpp
)

target_link_libraries(core_tests
  PRIVATE
    core
    GTest::gtest
    GTest::gtest_main
)

gtest_discover_tests(core_tests)