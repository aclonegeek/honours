add_executable(create_course scenarios/step_definitions.cpp scenarios/create_course.cpp)
add_executable(delete_course scenarios/step_definitions.cpp scenarios/delete_course.cpp)
add_executable(create_student scenarios/step_definitions.cpp scenarios/create_student.cpp)
add_executable(delete_student scenarios/step_definitions.cpp scenarios/delete_student.cpp)

add_executable(register_in_course scenarios/step_definitions.cpp scenarios/register_in_course.cpp)
add_executable(deregister_from_course scenarios/step_definitions.cpp scenarios/deregister_from_course.cpp)
add_executable(drop_course scenarios/step_definitions.cpp scenarios/drop_course.cpp)

target_link_libraries(create_course PRIVATE client server doctest Threads::Threads)
target_link_libraries(delete_course PRIVATE client server doctest Threads::Threads)
target_link_libraries(create_student PRIVATE client server doctest Threads::Threads)
target_link_libraries(delete_student PRIVATE client server doctest Threads::Threads)

target_link_libraries(register_in_course PRIVATE client server doctest Threads::Threads)
target_link_libraries(deregister_from_course PRIVATE client server doctest Threads::Threads)
target_link_libraries(drop_course PRIVATE client server doctest Threads::Threads)
