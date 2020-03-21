add_library(server
  server.cpp
  session/clerk_session.cpp
  session/student_session.cpp
  session/temporary_session.cpp)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(server PUBLIC message university utility asio Threads::Threads)

add_executable(run_server
  run_server.cpp
  session/clerk_session.cpp
  session/student_session.cpp
  session/temporary_session.cpp)
target_link_libraries(run_server PUBLIC server)
