cmake_minimum_required(VERSION 2.8)

project(Sandbox)

set(SRC_LIST src/Application.cpp)

add_subdirectory(engine)

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}  engine)
