cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME ProjectTransSimpleServer)
set(SIMPLE_SERVER_EXECUTABLE_NAME ${PROJECT_NAME}App)

project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

set(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

file(GLOB SIMPLE_SERVER_DANEJOE_SOURCES CONFIGURE_DEPENDS
    "${REPO_ROOT}/server/source/danejoe/common/binary/*.cpp"
    "${REPO_ROOT}/server/source/danejoe/common/core/*.cpp"
    "${REPO_ROOT}/server/source/danejoe/common/diagnostic/*.cpp"
    "${REPO_ROOT}/server/source/danejoe/common/system/*.cpp"
    "${REPO_ROOT}/server/source/danejoe/network/codec/*.cpp"
    "${REPO_ROOT}/server/source/danejoe/stringify/*.cpp"
)

add_executable(${SIMPLE_SERVER_EXECUTABLE_NAME}
    ${SIMPLE_SERVER_DANEJOE_SOURCES}
    "${CMAKE_CURRENT_LIST_DIR}/source/main.cpp"
)

target_include_directories(${SIMPLE_SERVER_EXECUTABLE_NAME} PRIVATE
    "${REPO_ROOT}/server/include"
    "${REPO_ROOT}/client/include"
)

target_compile_definitions(${SIMPLE_SERVER_EXECUTABLE_NAME} PRIVATE
    SIMPLE_SERVER_RESOURCE_DIR="${CMAKE_CURRENT_LIST_DIR}/resource"
)

target_link_libraries(${SIMPLE_SERVER_EXECUTABLE_NAME} PRIVATE Threads::Threads)
