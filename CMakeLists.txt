cmake_minimum_required(VERSION 3.14)

project (sast-benchmark
  LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/macros.cmake)

add_subdirectory(buffer-overread)
add_subdirectory(use-after-free)
add_subdirectory(double-free)
