project(sdftoy)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-local")
set(CMAKE_CXX_STANDARD 11)

include(ExternalProject)

find_package(OpenGL REQUIRED)

set(glfw_path modules/glfw)
add_subdirectory(${glfw_path})
include_directories(${glfw_path}/include)
set(link_libs glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})

add_executable(sdftoy main.cpp)
target_link_libraries(sdftoy ${link_libs})
