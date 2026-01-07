
cmake_minimum_required(VERSION 3.20)

find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

find_package(SQLite3 REQUIRED)

if(NOT TARGET ProjectTransCommonDaneJoe)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../common" "${CMAKE_BINARY_DIR}/ProjectTransCommon")
endif()

add_executable(ProjectTransClientTests
    source/repository/test_block_repository.cpp
    source/repository/test_client_file_repository.cpp
    source/repository/test_task_repository.cpp

    source/service/test_task_service.cpp

    ../source/repository/block_repository.cpp
    ../source/repository/client_file_repository.cpp
    ../source/repository/task_repository.cpp

    ../source/service/task_service.cpp
    ../source/model/entity/block_entity.cpp
)

target_include_directories(ProjectTransClientTests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
)

target_link_libraries(ProjectTransClientTests PRIVATE
    GTest::gtest_main
    ProjectTransCommonDaneJoe
    SQLite::SQLite3
)

target_compile_features(ProjectTransClientTests PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(ProjectTransClientTests)
