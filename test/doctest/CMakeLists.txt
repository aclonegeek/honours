enable_testing()

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
  scenarios/drop_course.cpp

  scenarios/native/ex1.cpp
  scenarios/native/ex2.cpp

  scenarios/parallel/ex1.cpp
  scenarios/parallel/ex2.cpp)

add_executable(test_suite ${SOURCE_FILES})
target_link_libraries(test_suite PRIVATE client server doctest Threads::Threads)

add_test(NAME ex1.0 COMMAND test_suite --tc=*EX1P1.0*)
add_test(NAME ex1.1 COMMAND test_suite --tc=*EX1P1.1*)
add_test(NAME ex1.2 COMMAND test_suite --tc=*EX1P1.2*)
add_test(NAME ex1.3 COMMAND test_suite --tc=*EX1P1.3*)
add_test(NAME ex1.4 COMMAND test_suite --tc=*EX1P1.4*)
add_test(NAME ex1.5 COMMAND test_suite --tc=*EX1P1.5*)

add_test(NAME ex2.0 COMMAND test_suite --tc=*EX2P1.0*)
add_test(NAME ex2.1 COMMAND test_suite --tc=*EX2P1.1*)
add_test(NAME ex2.2 COMMAND test_suite --tc=*EX2P1.2*)
add_test(NAME ex2.3 COMMAND test_suite --tc=*EX2P1.3*)
add_test(NAME ex2.4 COMMAND test_suite --tc=*EX2P1.4*)
add_test(NAME ex2.5 COMMAND test_suite --tc=*EX2P1.5*)
add_test(NAME ex2.6 COMMAND test_suite --tc=*EX2P1.6*)
