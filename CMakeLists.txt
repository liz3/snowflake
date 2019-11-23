cmake_minimum_required(VERSION 3.10)
project(snowflake C)

set(CMAKE_C_STANDARD 99)
find_package (Threads)

add_executable(snowflake main.c src/server.c src/server.h src/snowflake.c src/snowflake.h)
target_link_libraries (snowflake ${CMAKE_THREAD_LIBS_INIT})
