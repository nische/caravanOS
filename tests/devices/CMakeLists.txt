add_executable(device_tests
  test_wt901c.cpp
)

target_link_libraries(device_tests
    PRIVATE
    core
    hal
    devices
    GTest::gtest
    GTest::gtest_main
)

gtest_discover_tests(device_tests)