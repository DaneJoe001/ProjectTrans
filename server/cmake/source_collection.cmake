file(GLOB DANEJOE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/binary/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/diagnostic/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/system/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/concurrent/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/condition/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/database/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/logger/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/codec/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/context/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/event_loop/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/monitor/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/socket/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/url/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/stringify/*.cpp"
)

file(GLOB SERVER_MAIN_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/main/*.cpp"
)

list(FILTER SERVER_MAIN_DIR_SOURCES EXCLUDE REGEX ".*_main\\.cpp$")

set(SERVER_CORE_SOURCES
    ${SERVER_MAIN_DIR_SOURCES}
)

file(GLOB SERVER_NORMAL_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/protocol/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/connect/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/model/entity/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/model/transfer/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/model/view/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/repository/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/service/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/protocol/*.cpp"
)

file(GLOB SERVER_QOBJECT_DIR_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/include/view/**/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/main/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/common/util/*.hpp"
)

file(GLOB SERVER_QOBJECT_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/view/**/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/util/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/main/server_thread.cpp"
)

set(SERVER_QRC_SOURCES)

list(APPEND SERVER_CORE_SOURCES
    ${SERVER_NORMAL_DIR_SOURCES}
)

if(ADD_QT_LIB)
    list(APPEND SERVER_CORE_SOURCES
        ${SERVER_QOBJECT_DIR_SOURCES}
    )
    list(APPEND SERVER_CORE_SOURCES
        ${SERVER_QOBJECT_DIR_HEADERS}
    )
endif()
