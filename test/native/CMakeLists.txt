add_executable(register_in_course scenarios/register_in_course.cpp)

target_link_libraries(register_in_course PRIVATE server doctest Threads::Threads)
