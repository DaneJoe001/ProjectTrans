# 服务端依赖处理
if(ADD_QT_LIB)
    # @brief Qt6 组件列表
    set(QT_COMPONENTS "Core;Gui;Widgets;Sql;Network" CACHE STRING "Qt6 components to find and link (semicolon-separated list)")
    if(NOT TARGET Qt6::Core)
        if(DEFINED Qt6_DIR AND NOT Qt6_DIR STREQUAL "")
            find_package(Qt6 CONFIG REQUIRED COMPONENTS ${QT_COMPONENTS}
                NO_DEFAULT_PATH
                PATHS "${Qt6_DIR}"
            )
        else()
            find_package(Qt6 CONFIG REQUIRED COMPONENTS ${QT_COMPONENTS})
        endif()
    endif()
    if(TARGET ${SERVER_CORE_LIB_NAME})
        set_target_properties(${SERVER_CORE_LIB_NAME} PROPERTIES
            AUTOMOC ON
            AUTORCC ON
            AUTOUIC ON
        )
    endif()
    if(TARGET ${SERVER_EXECUTABLE_NAME})
        set_target_properties(${SERVER_EXECUTABLE_NAME} PROPERTIES
            AUTOMOC ON
            AUTORCC ON
            AUTOUIC ON
        )
    endif()
    # @brief 链接 Qt6 组件
    foreach(qt_component IN LISTS QT_COMPONENTS)
        target_link_libraries(${SERVER_CORE_LIB_NAME} PUBLIC Qt6::${qt_component})
    endforeach()
endif()

if(ADD_DANEJOE_LIB)
    find_package(SQLite3 REQUIRED)
    target_link_libraries(ProjectTransCommonDaneJoe PRIVATE SQLite::SQLite3)
endif()

find_package(OpenSSL REQUIRED)

target_link_libraries(${SERVER_CORE_LIB_NAME} PRIVATE OpenSSL::SSL)
target_link_libraries(${SERVER_CORE_LIB_NAME} PRIVATE OpenSSL::Crypto)
