cmake_minimum_required(VERSION 4.0.1)

project(ProjectTrans LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON )
set(CMAKE_AUTORCC ON )
set(CMAKE_AUTOUIC ON )

set(MOC_HEADERS 
    "include/view/main_window.hpp"
)

file(GLOB CLIENT_SOURCES
    "source/client/*.cpp"
    "source/view/*.cpp"
    "source/network/*.cpp"
)

file(GLOB SERVER_SOURCES
    "source/server/*.cpp"
    "source/network/*.cpp"
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt6_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

# 指定二进制目录
add_subdirectory(/home/danejoe001/personal_code/code_cpp_library/library_danejoe_log ${CMAKE_BINARY_DIR}/external/danejoe_log)

add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets
)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(client PRIVATE ${MOC_SOURCES})
target_link_libraries(client PRIVATE DaneJoeLog)



add_executable(server ${SERVER_SOURCES})
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server PRIVATE DaneJoeLog)