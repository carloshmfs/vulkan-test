cmake_minimum_required(VERSION 3.26.3)

project(vulkant-test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
    ${CMAKE_PROJECT_NAME} 
    src/main.cpp
)

# target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC libzip::zip)
