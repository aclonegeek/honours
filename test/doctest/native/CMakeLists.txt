set(SOURCE_FILES
  test_runner.cpp

  scenarios/step_definitions.cpp

  scenarios/login_logout.cpp
  scenarios/create_course.cpp
  scenarios/delete_course.cpp
  scenarios/create_student.cpp
  scenarios/delete_student.cpp

  scenarios/register_in_course.cpp
  scenarios/deregister_from_course.cpp
  scenarios/drop_course.cpp)

add_executable(test_suite ${SOURCE_FILES})
target_link_libraries(test_suite PRIVATE client server doctest Threads::Threads)
