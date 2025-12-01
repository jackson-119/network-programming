add_library(Logger STATIC
    Logger.cc
)

target_include_directories(Logger PUBLIC
    ${CMAKE_SOURCE_DIR}/include/common
    ${CMAKE_SOURCE_DIR}/3rdparty/spdlog/include
)

add_library(UdpConnector STATIC
    UdpConnector.cc
)
target_include_directories(UdpConnector PUBLIC
    ${CMAKE_SOURCE_DIR}/include/common
)
target_link_libraries(UdpConnector Logger)

if(WIN32)
    target_link_libraries(UdpConnector ws2_32)
endif()