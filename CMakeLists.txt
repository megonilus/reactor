cmake_minimum_required(VERSION 3.10)

project(
  reactor
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/backend/*.cpp)
file(GLOB_RECURSE TUI_SOURCES CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/tui/*.cpp)
file(GLOB_RECURSE SIMULATION CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/simulation/*.cpp)

add_executable(reactor-backend ${SOURCES})
add_executable(reactor-tui ${TUI_SOURCES})
add_executable(reactor-simulation ${SIMULATION})

target_include_directories(reactor-backend
                           PRIVATE ${CMAKE_SOURCE_DIR}/includes/backend)
target_include_directories(reactor-tui PRIVATE ${CMAKE_SOURCE_DIR}/includes/tui)
target_include_directories(reactor-simulation
                           PRIVATE ${CMAKE_SOURCE_DIR}/includes/simulation)
include_directories(${CMAKE_SOURCE_DIR}/includes/common)
