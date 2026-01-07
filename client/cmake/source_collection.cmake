# @brief 客户端主文件夹源文件
set(CLIENT_PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
file(GLOB CLIENT_MAIN_DIR_SOURCES CONFIGURE_DEPENDS
    "${CLIENT_PROJECT_DIR}/source/main/*.cpp"
)

# 排除客户端入口文件
list(FILTER CLIENT_MAIN_DIR_SOURCES EXCLUDE REGEX ".*_main\\.cpp$")

# 客户端源文件
set(CLIENT_CORE_SOURCES
    ${CLIENT_MAIN_DIR_SOURCES}
)

# 客户端通用源文件
file(GLOB CLIENT_NORMAL_DIR_SOURCES CONFIGURE_DEPENDS
    "${CLIENT_PROJECT_DIR}/source/model/entity/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/model/transfer/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/model/view/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/repository/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/service/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/controller/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/common/util/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/common/protocol/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/protocol/*.cpp"
)

# 客户端QObject对象系统头文件
file(GLOB CLIENT_QOBJECT_DIR_HEADERS CONFIGURE_DEPENDS
    "${CLIENT_PROJECT_DIR}/include/main/*.hpp"
    "${CLIENT_PROJECT_DIR}/include/controller/*.hpp"
    "${CLIENT_PROJECT_DIR}/include/view/**/*.hpp"
    "${CLIENT_PROJECT_DIR}/include/service/*.hpp"
    "${CLIENT_PROJECT_DIR}/include/connect/*.hpp"
    "${CLIENT_PROJECT_DIR}/include/context/*.hpp"
    "${CLIENT_PROJECT_DIR}/include/common/util/*.hpp"
)

# 客户端QObject对象系统源文件
file(GLOB CLIENT_QOBJECT_DIR_SOURCES CONFIGURE_DEPENDS
    "${CLIENT_PROJECT_DIR}/source/main/client_app.cpp"
    "${CLIENT_PROJECT_DIR}/source/view/**/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/controller/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/connect/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/context/*.cpp"
    "${CLIENT_PROJECT_DIR}/source/common/util/*.cpp"
)

file(GLOB CLIENT_QRC_SOURCES CONFIGURE_DEPENDS
    "${CLIENT_PROJECT_DIR}/resource/*.qrc"
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
