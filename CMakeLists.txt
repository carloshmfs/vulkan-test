cmake_minimum_required(VERSION 3.26.3)

project(vulkant-test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
    ${CMAKE_PROJECT_NAME} 
    src/TriangleExampleApp.cpp
    src/main.cpp
)

find_package(glm REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glm::glm)

find_package(glfw3 3.4 REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glfw)

find_package(Vulkan REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC Vulkan::Vulkan)
