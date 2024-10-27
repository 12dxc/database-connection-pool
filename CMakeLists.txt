cmake_minimum_required(VERSION 3.20)
project(test)
set(CMAKE_CXX_STANDARD 23)

include_directories(thirdparty/)
aux_source_directory(. SRC)
aux_source_directory("./thirdparty/" TSRC)

add_executable(${PROJECT_NAME} ${SRC} ${TSRC})
target_link_libraries(${PROJECT_NAME} mysqlclient)