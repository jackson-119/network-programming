set(QML_RESOURCES ${CMAKE_SOURCE_DIR}/resources.qrc)
set(APP_SOURCES
    app.cc)

qt_add_executable(${PROJECT_NAME} 
    ${APP_SOURCES}
    ${QML_RESOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Quick
    Logger
    Client
    Protocol
    Message
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include/client
    ${CMAKE_SOURCE_DIR}/include/common
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_SOURCE_DIR}/bin/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/PeerChat $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            ${CMAKE_SOURCE_DIR}/bin/
    COMMAND_EXPAND_LISTS
)