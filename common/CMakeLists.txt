file(GLOB PROJECT_TRANS_COMMON_DANEJOE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/common/binary/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/common/core/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/common/diagnostic/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/common/handle/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/common/status/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/common/system/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/concurrent/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/concurrent/timer/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/condition/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/database/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/logger/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/network/codec/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/network/context/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/network/event_loop/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/network/handle/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/network/runtime/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/network/status/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/network/url/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/source/danejoe/stringify/*.cpp"
)

add_library(ProjectTransCommonDaneJoe ${PROJECT_TRANS_COMMON_DANEJOE_SOURCES})

target_include_directories(ProjectTransCommonDaneJoe PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

target_compile_features(ProjectTransCommonDaneJoe PUBLIC cxx_std_20)
