cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 11)

# set the project name
project(reliable_transport_layer)


# define verbose for packet dump
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSPDLOG_ACTIVE_LEVEL=1")


#Fetch GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()


#Fetch spdlog
#https://github.com/gabime/spdlog/issues/1520
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.9.2
)
FetchContent_MakeAvailable(spdlog)

add_library(
  infrastructure 
  "src/infrastructure/UnreliableNetworkLayer/unreliable_network_layer.cpp"
  "src/infrastructure/ReliableTransportLayerImplementations/go_back_n.cpp"
  "src/infrastructure/helpers/crc32.cpp"
)

target_link_libraries(infrastructure spdlog::spdlog)

add_executable(
  tests
        tests/unreliable_network_layer_test.cpp
        tests/gbn_test.cpp
        tests/entry.cpp
)
add_executable(
        monitor
        src/monitor/entry.cpp
)

target_link_libraries(
  tests
  gtest_main
  infrastructure
)

target_link_libraries(monitor infrastructure)

include(GoogleTest)
include_directories(include)
gtest_discover_tests(tests)
