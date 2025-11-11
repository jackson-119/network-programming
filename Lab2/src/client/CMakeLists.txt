add_library(Client Client.cc  ${CMAKE_SOURCE_DIR}/include/client/Client.h)

target_include_directories(
  Client
  PUBLIC ${CMAKE_SOURCE_DIR}/include/protocol
         ${CMAKE_SOURCE_DIR}/include/client ${CMAKE_SOURCE_DIR}/include/common)

target_link_libraries(Client PRIVATE Protocol Logger UdpConnector Qt6::Core Qt6::Quick)