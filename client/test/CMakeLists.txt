
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

add_executable(ProjectTransClientTests
    source/repository/test_block_repository.cpp
    source/repository/test_client_file_repository.cpp
    source/repository/test_task_repository.cpp

    ../source/repository/block_repository.cpp
    ../source/repository/client_file_repository.cpp
    ../source/repository/task_repository.cpp
    ../source/model/entity/block_entity.cpp

    ../../server/source/danejoe/common/core/data_type.cpp
    ../../server/source/danejoe/common/diagnostic/diagnostic_event.cpp
    ../../server/source/danejoe/common/diagnostic/diagnostic_system.cpp
    ../../server/source/danejoe/common/system/system_info.cpp

    ../../server/source/danejoe/database/i_sql_stringify.cpp
    ../../server/source/danejoe/database/sql_builder.cpp
    ../../server/source/danejoe/database/sql_database.cpp
    ../../server/source/danejoe/database/sql_database_manager.cpp
    ../../server/source/danejoe/database/sql_query.cpp
    ../../server/source/danejoe/database/sql_schema.cpp
    ../../server/source/danejoe/database/sql_table_query.cpp
    ../../server/source/danejoe/database/sqlite_driver.cpp
    ../../server/source/danejoe/database/sqlite_stringify.cpp

    ../../server/source/danejoe/logger/async_logger.cpp
    ../../server/source/danejoe/logger/i_logger.cpp
    ../../server/source/danejoe/logger/logger_config.cpp
    ../../server/source/danejoe/logger/logger_manager.cpp
)

target_include_directories(ProjectTransClientTests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
    ${CMAKE_CURRENT_LIST_DIR}/../../server/include
)

target_link_libraries(ProjectTransClientTests PRIVATE
    GTest::gtest_main
    SQLite::SQLite3
)

target_compile_features(ProjectTransClientTests PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(ProjectTransClientTests)
