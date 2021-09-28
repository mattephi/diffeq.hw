cmake_minimum_required(VERSION 3.20)
project(DE_Plot)

set(CMAKE_CXX_STANDARD 20)
set(SOURCE_FILES src/main.cpp src/glad.c)

find_package(glfw3 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(DE_Plot ${SOURCE_FILES})
target_link_libraries(DE_Plot glfw)

