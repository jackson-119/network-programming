add_library(Protocol STATIC
    Protocol.cc
)

target_include_directories(Protocol PUBLIC
    ${CMAKE_SOURCE_DIR}/include/protocol
    ${CMAKE_SOURCE_DIR}/include/common
    ${CMAKE_SOURCE_DIR}/include/message
)
target_link_libraries(Protocol Message Logger)