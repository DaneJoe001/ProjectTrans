
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

add_executable(ProjectTransServerTests
    source/common/error/test_error_code.cpp
    source/common/handle/test_unique_handle.cpp
    source/common/status/test_status_code.cpp

    ../source/danejoe/common/handle/unique_handle.cpp
    ../source/danejoe/common/status/i_status_detail.cpp
    ../source/danejoe/common/status/status_code.cpp
    ../source/danejoe/common/status/posix_status_detail.cpp
    ../source/danejoe/network/status/posix_status_code.cpp
)

target_include_directories(ProjectTransServerTests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
)

target_link_libraries(ProjectTransServerTests PRIVATE
    GTest::gtest_main
)

target_compile_features(ProjectTransServerTests PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(ProjectTransServerTests)
