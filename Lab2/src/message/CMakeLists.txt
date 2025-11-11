add_library(Message STATIC
    BasicMessage.cc
)

target_include_directories(Message PUBLIC
    ${CMAKE_SOURCE_DIR}/include/message
    ${CMAKE_SOURCE_DIR}/3rdparty/cereal/include
)