cmake_minimum_required(VERSION 3.30)
project(PeerChat)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Quick REQUIRED)

# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

add_subdirectory(3rdparty)
add_subdirectory(src/common)
add_subdirectory(src/message)
add_subdirectory(src/protocol)
add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(src/app)

# 拷贝server到bin目录
