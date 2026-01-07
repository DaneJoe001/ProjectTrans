cmake_minimum_required(VERSION 3.20)

project(ProjectTransSuperbuild LANGUAGES CXX)

option(BUILD_CLIENT_SUBPROJECT "Build client subproject" ON)
option(BUILD_SERVER_SUBPROJECT "Build server subproject" ON)

add_subdirectory(common)

if(CMAKE_EXPORT_COMPILE_COMMANDS AND CMAKE_GENERATOR MATCHES "Ninja|Makefiles")
  if(NOT TARGET update_compile_commands)
    add_custom_target(update_compile_commands ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              "${CMAKE_BINARY_DIR}/compile_commands.json"
              "${CMAKE_SOURCE_DIR}/compile_commands.json")
  endif()
endif()

if(BUILD_CLIENT_SUBPROJECT)
    add_subdirectory(client)
endif()

if(BUILD_SERVER_SUBPROJECT)
    add_subdirectory(server)
endif()
