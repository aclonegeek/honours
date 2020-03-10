cmake_minimum_required(VERSION 3.16)

project(honours)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(server
  src/util.cpp

  src/course.cpp
  src/student.cpp
  src/university.cpp

  src/server.cpp
  src/message.cpp
  src/clerk_session.cpp
  src/student_session.cpp
  src/temporary_session.cpp)

add_executable(client
  src/client.cpp
  src/message.cpp)

find_package(Threads)
target_link_libraries(server Threads::Threads)
target_link_libraries(client Threads::Threads)
