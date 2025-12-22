file(GLOB CLIENT_MAIN_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/client/main/*.cpp"
)

list(FILTER CLIENT_MAIN_DIR_SOURCES EXCLUDE REGEX ".*_main\\.cpp$")

set(CLIENT_CORE_SOURCES
    ${CLIENT_MAIN_DIR_SOURCES}
)

file(GLOB CLIENT_NORMAL_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/client/model/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/client/repository/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/client/service/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/binary/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/diagnostic/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/system/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/codec/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/context/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/event_loop/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/monitor/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/socket/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/url/*.cpp"
    # "${CMAKE_SOURCE_DIR}/source/common/database/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/util/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/protocol/*.cpp"
    # "${CMAKE_SOURCE_DIR}/source/common/log/*.cpp"
    # "${CMAKE_SOURCE_DIR}/source/common/mt_queue/*.cpp"
)

list(FILTER CLIENT_NORMAL_DIR_SOURCES EXCLUDE REGEX ".*byte_array_util\\.cpp$")
list(FILTER CLIENT_NORMAL_DIR_SOURCES EXCLUDE REGEX ".*enum_util\\.cpp$")
list(FILTER CLIENT_NORMAL_DIR_SOURCES EXCLUDE REGEX ".*util_process\\.cpp$")

file(GLOB CLIENT_QOBJECT_DIR_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/include/client/main/client_app.hpp"
    "${CMAKE_SOURCE_DIR}/include/client/view/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/client/connect/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/common/util/screen_util.hpp"
)

file(GLOB CLIENT_QOBJECT_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/client/main/client_app.cpp"
    "${CMAKE_SOURCE_DIR}/source/client/view/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/client/connect/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/util/*.cpp"
)

list(FILTER CLIENT_QOBJECT_DIR_SOURCES EXCLUDE REGEX ".*byte_array_util\\.cpp$")
list(FILTER CLIENT_QOBJECT_DIR_SOURCES EXCLUDE REGEX ".*enum_util\\.cpp$")
list(FILTER CLIENT_QOBJECT_DIR_SOURCES EXCLUDE REGEX ".*util_process\\.cpp$")

file(GLOB CLIENT_QRC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/resource/client/*.qrc"
)

list(APPEND CLIENT_CORE_SOURCES
    ${CLIENT_NORMAL_DIR_SOURCES}
)

if(ADD_QT_LIB)
    list(APPEND CLIENT_CORE_SOURCES
        ${CLIENT_QOBJECT_DIR_SOURCES}
    )
    list(APPEND CLIENT_CORE_SOURCES
        ${CLIENT_QOBJECT_DIR_HEADERS}
    )
endif()

file(GLOB SERVER_MAIN_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/server/main/*.cpp"
)

list(FILTER SERVER_MAIN_DIR_SOURCES EXCLUDE REGEX ".*_main\\.cpp$")

set(SERVER_CORE_SOURCES
    ${SERVER_MAIN_DIR_SOURCES}
)

file(GLOB SERVER_NORMAL_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/server/connect/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/server/model/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/server/repository/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/server/service/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/binary/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/diagnostic/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/common/system/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/codec/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/context/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/event_loop/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/monitor/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/socket/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/danejoe/network/url/*.cpp"
    # "${CMAKE_SOURCE_DIR}/source/common/database/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/protocol/*.cpp"
    # "${CMAKE_SOURCE_DIR}/source/common/log/*.cpp"
    # "${CMAKE_SOURCE_DIR}/source/common/mt_queue/*.cpp"
)

list(FILTER SERVER_NORMAL_DIR_SOURCES EXCLUDE REGEX ".*byte_array_util\\.cpp$")
list(FILTER SERVER_NORMAL_DIR_SOURCES EXCLUDE REGEX ".*enum_util\\.cpp$")
list(FILTER SERVER_NORMAL_DIR_SOURCES EXCLUDE REGEX ".*util_process\\.cpp$")

file(GLOB SERVER_QOBJECT_DIR_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/include/server/view/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/server/main/server_app.hpp"
    "${CMAKE_SOURCE_DIR}/include/server/main/server_thread.hpp"
    "${CMAKE_SOURCE_DIR}/include/common/util/screen_util.hpp"
)

file(GLOB SERVER_QOBJECT_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/server/view/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/util/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/server/main/server_thread.cpp"
    "${CMAKE_SOURCE_DIR}/source/server/main/widget_main.cpp"
)

list(FILTER SERVER_QOBJECT_DIR_SOURCES EXCLUDE REGEX ".*byte_array_util\\.cpp$")
list(FILTER SERVER_QOBJECT_DIR_SOURCES EXCLUDE REGEX ".*enum_util\\.cpp$")
list(FILTER SERVER_QOBJECT_DIR_SOURCES EXCLUDE REGEX ".*util_process\\.cpp$")

file(GLOB SERVER_QRC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/resource/server/*.qrc"
)

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
