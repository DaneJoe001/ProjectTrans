cmake_minimum_required(VERSION 4.0.1)

project(ProjectTrans LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON )
set(CMAKE_AUTORCC ON )
set(CMAKE_AUTOUIC ON )

file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS
    "include/common/util/screen_util.hpp"
    "include/client/view/*.hpp"
    "include/client/connect/*.hpp"
    "source/client/connect/*.cpp"
    "source/client/main/*.cpp"
    "source/client/model/*.cpp"
    "source/client/repository/*.cpp"
    "source/client/service/*.cpp"
    "source/client/view/*.cpp"
    "source/common/network/*.cpp"
    "source/common/database/*.cpp"
    "source/common/util/*.cpp"
    "source/common/protocol/*.cpp"
)

file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS
    "include/common/util/screen_util.hpp"
    "include/server/view/*.hpp"
    "source/server/connect/*.cpp"
    "source/server/main/*.cpp"
    "source/server/model/*.cpp"
    "source/server/repository/*.cpp"
    "source/server/service/*.cpp"
    "source/server/view/*.cpp"
    "source/common/network/*.cpp"
    "source/common/database/*.cpp"
    "source/common/util/*.cpp"
    "source/common/protocol/*.cpp"
)

set(BUILD_CLIENT on)
set(BUILD_SERVER on)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(OpenSSL REQUIRED)

# 使用add_subdirectory方式添加SQLiteCpp
add_subdirectory(/home/danejoe001/develop_environment/environment_database/database_sqlite/SQLiteCpp ${CMAKE_BINARY_DIR}/external/sqlitecpp)

# 指定二进制目录
add_subdirectory(/home/danejoe001/personal_code/code_cpp_library/library_danejoe_log ${CMAKE_BINARY_DIR}/external/danejoe_log)
add_subdirectory(/home/danejoe001/personal_code/code_cpp_library/library_danejoe_mt_queue ${CMAKE_BINARY_DIR}/external/danejoe_mt_queue)


if(BUILD_CLIENT)
    add_executable(client ${CLIENT_SOURCES})
    target_link_libraries(client PRIVATE 
        Qt6::Core 
        Qt6::Gui 
        Qt6::Widgets
    )
    target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(client PRIVATE DaneJoeLog)
    target_link_libraries(client PRIVATE DaneJoeMTQueue)
    target_link_libraries(client PRIVATE SQLiteCpp)
    target_link_libraries(client PRIVATE OpenSSL::SSL)
    target_link_libraries(client PRIVATE OpenSSL::Crypto)
endif()

if(BUILD_SERVER)
    add_executable(server ${SERVER_SOURCES})
    target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(server PRIVATE DaneJoeLog)
    target_link_libraries(server PRIVATE    DaneJoeMTQueue)
    target_link_libraries(server PRIVATE 
        Qt6::Core 
        Qt6::Gui 
        Qt6::Widgets
    )
    target_link_libraries(server PRIVATE SQLiteCpp)
    target_link_libraries(server PRIVATE OpenSSL::SSL)
    target_link_libraries(server PRIVATE    OpenSSL::Crypto)
endif()
