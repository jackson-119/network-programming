add_executable(server Server.cc )

target_include_directories(
  server
  PUBLIC ${CMAKE_SOURCE_DIR}/include/protocol
         ${CMAKE_SOURCE_DIR}/include/server ${CMAKE_SOURCE_DIR}/include/common)

target_link_libraries(server PRIVATE Protocol Logger UdpConnector)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_custom_command(
  TARGET server
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:server>
          ${CMAKE_SOURCE_DIR}/bin/
  COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:server>>,copy_if_different,"echo"> $<TARGET_RUNTIME_DLLS:server>
          $<$<BOOL:$<TARGET_RUNTIME_DLLS:server>>:${CMAKE_SOURCE_DIR}/bin/>
  COMMAND_EXPAND_LISTS
)
