cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(HEADERS_DIR ./include)
set(SRC_DIR ./src)

set(HEADERS
    ${HEADERS_DIR}/client.hpp
    ${HEADERS_DIR}/club.hpp
    ${HEADERS_DIR}/event.hpp
    ${HEADERS_DIR}/handler.hpp
    ${HEADERS_DIR}/table.hpp
    ${HEADERS_DIR}/timestamp.hpp
    )

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/client.cpp
    ${SRC_DIR}/club.cpp
    ${SRC_DIR}/event.cpp
    ${SRC_DIR}/handler.cpp
    ${SRC_DIR}/table.cpp
    ${SRC_DIR}/timestamp.cpp
    )

project(yadro_ex LANGUAGES CXX)

add_executable(main.exe ${SOURCES} ${HEADERS})