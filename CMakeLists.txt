cmake_minimum_required(VERSION 3.15)
project(snowflake C)

set(CMAKE_C_STANDARD 99)

add_executable(snowflake main.c src/server.c src/server.h src/snowflake.c src/snowflake.h)