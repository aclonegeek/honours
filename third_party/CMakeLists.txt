add_library(asio INTERFACE)
target_compile_options(asio INTERFACE -DASIO_STANDALONE)
if(WIN32)
  target_compile_options(asio INTERFACE -D_WIN32_WINNT=0x0601)
endif()
target_include_directories(asio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/asio/asio/include)
target_link_directories(asio INTERFACE Threads::Threads)

add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/doctest/doctest)
