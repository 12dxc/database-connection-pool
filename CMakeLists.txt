cmake_minimum_required(VERSION 3.20)

project(test)

set(CMAKE_CXX_STANDARD 23)

aux_source_directory(. SRC)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} stdc++exp mysqlclient)