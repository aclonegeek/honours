add_library(client client.cpp)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(client PUBLIC message Threads::Threads)

add_executable(run_client run_client.cpp)
target_link_libraries(run_client PUBLIC client)
