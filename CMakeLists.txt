cmake_minimum_required(VERSION 3.20)

project(ProjectTransSuperbuild LANGUAGES CXX)

option(BUILD_CLIENT_SUBPROJECT "Build client subproject" ON)
option(BUILD_SERVER_SUBPROJECT "Build server subproject" ON)
option(BUILD_SIMPLE_SERVER "Build simple_server subproject" ON)

if(BUILD_CLIENT_SUBPROJECT)
    add_subdirectory(client)
endif()

if(BUILD_SERVER_SUBPROJECT)
    add_subdirectory(server)
endif()

if(BUILD_SIMPLE_SERVER)
    add_subdirectory(simple_server)
endif()
