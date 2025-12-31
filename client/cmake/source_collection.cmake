# @brief 客户端主文件夹源文件
file(GLOB CLIENT_MAIN_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/main/*.cpp"
)

file(GLOB CLIENT_DANEJOE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/common/binary/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/common/core/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/common/diagnostic/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/common/system/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/concurrent/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/concurrent/timer/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/condition/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/database/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/logger/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/network/codec/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/network/context/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/network/event_loop/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/network/monitor/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/network/socket/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/network/url/*.cpp"
    "${CMAKE_SOURCE_DIR}/../server/source/danejoe/stringify/*.cpp"
)

# 排除客户端入口文件
list(FILTER CLIENT_MAIN_DIR_SOURCES EXCLUDE REGEX ".*_main\\.cpp$")

# 客户端源文件
set(CLIENT_CORE_SOURCES
    ${CLIENT_MAIN_DIR_SOURCES}
)

# 客户端通用源文件
file(GLOB CLIENT_NORMAL_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/model/entity/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/model/transfer/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/model/view/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/repository/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/service/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/controller/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/util/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/protocol/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/protocol/*.cpp"
)

# 客户端QObject对象系统头文件
file(GLOB CLIENT_QOBJECT_DIR_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/include/main/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/controller/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/view/**/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/service/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/connect/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/context/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/common/util/*.hpp"
)

# 客户端QObject对象系统源文件
file(GLOB CLIENT_QOBJECT_DIR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/main/client_app.cpp"
    "${CMAKE_SOURCE_DIR}/source/view/**/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/controller/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/connect/*.cpp"
    "${CMAKE_SOURCE_DIR}/source/common/util/*.cpp"
)

file(GLOB CLIENT_QRC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/resource/*.qrc"
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
