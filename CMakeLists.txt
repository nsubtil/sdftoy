project(sdftoy)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-local")
set(CMAKE_CXX_STANDARD 11)

include(ExternalProject)

find_package(OpenGL REQUIRED)

set(glfw_path modules/glfw)
add_subdirectory(${glfw_path})
include_directories(${glfw_path}/include gen-glad/include)
set(link_libs glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})

file(GLOB_RECURSE shader_files RECURSIVE "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")

add_custom_command(OUTPUT shader_map.gen.cpp
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/shadergen.py ${CMAKE_CURRENT_SOURCE_DIR}/shaders > ${CMAKE_CURRENT_BINARY_DIR}/shader_map.gen.cpp
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shadergen.py ${shader_files})

add_executable(sdftoy main.cpp ${CMAKE_CURRENT_BINARY_DIR}/shader_map.gen.cpp gen-glad/src/glad.c)
target_link_libraries(sdftoy ${link_libs})
