cmake_minimum_required(VERSION 2.8)
project(Sludge)

# Add sources
file (GLOB SOURCES src/*.cpp)

# Includes
include_directories(include/)

# Flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2 -pthread -g")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O2 -std=c++17 -pthread -g")

# Build
add_executable(${PROJECT_NAME} ${SOURCES})

