add_executable(server
  server.cpp
  session/clerk_session.cpp
  session/student_session.cpp
  session/temporary_session.cpp)

target_link_libraries(server PRIVATE message university utility Threads::Threads)
