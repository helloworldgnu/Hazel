cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)

project(Sandbox)

set(SRC_LIST 
    src/SandBoxApp.cpp
    src/Renderer2D.cpp
    src/Renderer2D.h)

add_subdirectory(Hazel)

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}  hazel)
target_include_directories(${PROJECT_NAME} PRIVATE Hazel/src)