cmake_minimum_required(VERSION 3.10)

project(
  reactor
  VERSION 0.0.1
  LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Wpedantic)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_EXTENSIONS OFF) # strict compiler's rules

set(CMAKE_BUILD_TYPE Release)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/backend/*.cpp)
file(GLOB_RECURSE TUI_SOURCES CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/tui/*.cpp)
file(GLOB_RECURSE SIMULATION CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/simulation/*.cpp)

add_executable(reactor-backend ${SOURCES})
add_executable(reactor-tui ${TUI_SOURCES})
add_executable(reactor-simulation ${SIMULATION})

add_subdirectory(external/tomlplusplus)
target_link_libraries(reactor-backend PRIVATE tomlplusplus)

target_include_directories(reactor-backend
                           PRIVATE ${CMAKE_SOURCE_DIR}/includes/backend)
target_include_directories(reactor-tui PRIVATE ${CMAKE_SOURCE_DIR}/includes/tui)
target_include_directories(reactor-simulation
                           PRIVATE ${CMAKE_SOURCE_DIR}/includes/simulation)

include_directories(${CMAKE_SOURCE_DIR}/includes/common)
