cmake_minimum_required(VERSION 3.26.3)

project(vulkan-test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS -fdiagnostics-color=always)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(GLSLC glslc HINTS 
    /usr/bin 
    /usr/local/bin 
    $ENV{VULKAN_SDK}/Bin/ 
    $ENV{VULKAN_SDK}/Bin32/)

if (NOT GLSLC)
    message(FATAL_ERROR "glslc not found at ${GLSLC}")
endif ()

function(add_shaders TARGET_NAME)
    set(SHADER_SOURCE_FILES ${ARGN}) 

    list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
    if(FILE_COUNT EQUAL 0)
        message(FATAL_ERROR "Cannot create a shaders target without any source files")
    endif()

    make_directory("${CMAKE_CURRENT_BINARY_DIR}/shaders")

    set(SHADER_COMMANDS_LIST "") 

    foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)

        get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME)

        string(REPLACE ".vert" ".spv" OUTPUT_NAME "${SHADER_NAME}")
        string(REPLACE ".frag" ".spv" OUTPUT_NAME "${SHADER_NAME}")
        string(REPLACE ".glsl" ".spv" OUTPUT_NAME "${SHADER_NAME}")

        set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv")

        add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND "${GLSLC}" ${SHADER_SOURCE} -o ${OUTPUT_FILE}
            DEPENDS ${SHADER_SOURCE}
            COMMENT "Building Shader object [${OUTPUT_FILE}]"
            VERBATIM 
        )

        list(APPEND SHADER_COMMANDS_LIST ${OUTPUT_FILE})
    endforeach()

    add_custom_target(${TARGET_NAME} ALL DEPENDS ${SHADER_COMMANDS_LIST})
endfunction()

add_shaders(
    TriangleShaders 
    "shaders/triangle.vert" 
    "shaders/triangle.frag"
)

add_executable(
    ${CMAKE_PROJECT_NAME} 
    "src/TriangleExampleApp.cpp"
    "src/TriangleExampleApp.h"
    "src/helpers.cpp"
    "src/helpers.h"
    "src/main.cpp"
)

find_package(glm REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glm::glm)

find_package(glfw3 3.4 REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glfw)

find_package(Vulkan REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC Vulkan::Vulkan)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
