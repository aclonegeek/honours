cmake_minimum_required(VERSION 3.16)

project(honours LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Werror")
endif()

find_package(Threads REQUIRED)

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(test)
