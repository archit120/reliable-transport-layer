cmake_minimum_required(VERSION 3.14)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

# set the project name
project(reliable_transport_layer)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()


add_library(infrastructure "src/infrastructure/UnreliableNetworkLayer/unreliable_network_layer.cpp")

add_executable(
  tests
  tests/unreliable_network_layer_test.cpp
)
target_link_libraries(
  tests
  gtest_main
  infrastructure
)

include(GoogleTest)
include_directories(include)
gtest_discover_tests(tests)
