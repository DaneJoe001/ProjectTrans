cmake_minimum_required(VERSION 3.20)

# @brief 项目名称
set(PROJECT_NAME ProjectTransClient)

# @brief 核心库名称（避免与其他子工程 target 重名）
set(CLIENT_CORE_LIB_NAME ${PROJECT_NAME}Core)
# @brief 可执行文件名称（避免与其他子工程 target 重名）
set(CLIENT_EXECUTABLE_NAME ${PROJECT_NAME}App)

project(${PROJECT_NAME} LANGUAGES CXX)

option(CLIENT_TEST_ONLY "Build only client tests" OFF)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/options.cmake")

if(CLIENT_TEST_ONLY)
    set(BUILD_TEST ON CACHE BOOL "" FORCE)
    include(CTest)
    enable_testing()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
        add_subdirectory(test)
    endif()
    return()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/cmake/project_options.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/warnings.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/source_collection.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/entry_points.cmake")

if(NOT TARGET ProjectTransCommonDaneJoe)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../common" "${CMAKE_BINARY_DIR}/ProjectTransCommon")
endif()
apply_warnings(ProjectTransCommonDaneJoe)

add_library(${CLIENT_CORE_LIB_NAME} ${CLIENT_CORE_SOURCES})

target_include_directories(${CLIENT_CORE_LIB_NAME} PUBLIC "include")
target_link_libraries(${CLIENT_CORE_LIB_NAME} PUBLIC ProjectTransCommonDaneJoe)
apply_warnings(${CLIENT_CORE_LIB_NAME})

add_executable(${CLIENT_EXECUTABLE_NAME} ${CLIENT_EXECUTABLE_SOURCE} ${CLIENT_QRC_SOURCES})
target_link_libraries(${CLIENT_EXECUTABLE_NAME} PRIVATE ${CLIENT_CORE_LIB_NAME})
apply_warnings(${CLIENT_EXECUTABLE_NAME})

include("${CMAKE_CURRENT_LIST_DIR}/cmake/dependencies.cmake")

if(BUILD_TEST)
    include(CTest)
    enable_testing()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
        add_subdirectory(test)
    endif()
endif()
