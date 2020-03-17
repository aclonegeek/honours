set(SOURCE_FILES
  course.hpp
  course.cpp
  student.hpp
  student.cpp
  university.hpp
  university.cpp)

add_library(university SHARED STATIC ${SOURCE_FILES})
target_include_directories(university PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
