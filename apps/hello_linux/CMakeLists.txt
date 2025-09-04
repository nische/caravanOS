# apps/hello_linux/CMakeLists.txt
add_executable(hello_linux main.cpp)
target_link_libraries(hello_linux PRIVATE hal)
