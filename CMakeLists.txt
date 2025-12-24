cmake_minimum_required(VERSION 3.20)

# @brief 项目名称
set(PROJECT_NAME ProjectTrans)
# @brief 核心库名称
set(CLIENT_CORE_LIB_NAME ClientCore)
# @brief 核心库名称
set(SERVER_CORE_LIB_NAME ServerCore)
# @brief 可执行文件名称
set(CLIENT_EXECUTABLE_NAME client)
# @brief 可执行文件名称
set(SERVER_EXECUTABLE_NAME server)

project(${PROJECT_NAME} LANGUAGES CXX)

include("${CMAKE_SOURCE_DIR}/cmake/options.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/project_options.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/warnings.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/source_collection.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/entry_points.cmake")

add_library(DaneJoe ${DANEJOE_SOURCES})
target_include_directories(DaneJoe PUBLIC "include")
apply_warnings(DaneJoe)

add_library(${CLIENT_CORE_LIB_NAME} ${CLIENT_CORE_SOURCES})
target_include_directories(${CLIENT_CORE_LIB_NAME} PUBLIC "include")
target_link_libraries(${CLIENT_CORE_LIB_NAME} PUBLIC DaneJoe)
apply_warnings(${CLIENT_CORE_LIB_NAME})

add_library(${SERVER_CORE_LIB_NAME} ${SERVER_CORE_SOURCES})
target_include_directories(${SERVER_CORE_LIB_NAME} PUBLIC "include")
target_link_libraries(${SERVER_CORE_LIB_NAME} PUBLIC DaneJoe)
apply_warnings(${SERVER_CORE_LIB_NAME})

add_executable(${CLIENT_EXECUTABLE_NAME} ${CLIENT_EXECUTABLE_SOURCE} ${CLIENT_QRC_SOURCES})
target_link_libraries(${CLIENT_EXECUTABLE_NAME} PRIVATE ${CLIENT_CORE_LIB_NAME})
apply_warnings(${CLIENT_EXECUTABLE_NAME})

add_executable(${SERVER_EXECUTABLE_NAME} ${SERVER_EXECUTABLE_SOURCE} ${CLIENT_QRC_SOURCES})
target_link_libraries(${SERVER_EXECUTABLE_NAME} PRIVATE ${SERVER_CORE_LIB_NAME})
apply_warnings(${SERVER_EXECUTABLE_NAME})

include("${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake")

if(BUILD_TEST)
    # @brief 启用测试
    include(CTest)
    enable_testing()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
      # @brief 添加测试子目录
      add_subdirectory(test)
    endif()
endif()
