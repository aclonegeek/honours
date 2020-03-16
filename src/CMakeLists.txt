add_executable(server
  util.cpp

  course.cpp
  student.cpp
  university.cpp

  server.cpp
  message.cpp
  clerk_session.cpp
  student_session.cpp
  temporary_session.cpp)

add_executable(client
  client.cpp
  message.cpp)

target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE Threads::Threads)
