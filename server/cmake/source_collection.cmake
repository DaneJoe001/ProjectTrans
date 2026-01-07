set(SERVER_PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")

file(GLOB SERVER_MAIN_DIR_SOURCES CONFIGURE_DEPENDS
    "${SERVER_PROJECT_DIR}/source/main/*.cpp"
)

list(FILTER SERVER_MAIN_DIR_SOURCES EXCLUDE REGEX ".*_main\\.cpp$")

set(SERVER_CORE_SOURCES
    ${SERVER_MAIN_DIR_SOURCES}
)

file(GLOB SERVER_NORMAL_DIR_SOURCES CONFIGURE_DEPENDS
    "${SERVER_PROJECT_DIR}/source/protocol/*.cpp"
    "${SERVER_PROJECT_DIR}/source/model/entity/*.cpp"
    "${SERVER_PROJECT_DIR}/source/model/transfer/*.cpp"
    "${SERVER_PROJECT_DIR}/source/model/view/*.cpp"
    "${SERVER_PROJECT_DIR}/source/repository/*.cpp"
    "${SERVER_PROJECT_DIR}/source/service/*.cpp"
    "${SERVER_PROJECT_DIR}/source/runtime/*.cpp"
)

file(GLOB SERVER_QOBJECT_DIR_HEADERS CONFIGURE_DEPENDS
    "${SERVER_PROJECT_DIR}/include/view/**/*.hpp"
    "${SERVER_PROJECT_DIR}/include/main/*.hpp"
    "${SERVER_PROJECT_DIR}/include/common/util/*.hpp"
)

file(GLOB SERVER_QOBJECT_DIR_SOURCES CONFIGURE_DEPENDS
    "${SERVER_PROJECT_DIR}/source/view/**/*.cpp"
    "${SERVER_PROJECT_DIR}/source/common/util/*.cpp"
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
