# -- FTXUI asks for 3.22
cmake_minimum_required(VERSION 3.22)

project(
  reactor
  VERSION 0.0.1
  LANGUAGES CXX)

if(MSVC)
  add_compile_options(/W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -- Configuring
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DEFAULT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/includes")

# -- Common
include_directories(${CMAKE_SOURCE_DIR}/includes/common)

# -- Subdirectories(libraries)
add_subdirectory(src/config)
add_subdirectory(src/tui)
add_subdirectory(src/backend)

# -- Binary
add_executable(reactor src/main.cpp)

# -- Static linking
target_link_libraries(reactor PRIVATE reactor-backend reactor-tui)
