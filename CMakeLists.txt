cmake_minimum_required(VERSION 3.22)

project(
  reactor
  VERSION 0.0.1
  LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Wpedantic)

# -- Some options
set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_COMPILER clang++)  CMake will decide

# set(CMAKE_C_COMPILER clang)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -- FXTUI
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/arthursonzogni/ftxui.git
  GIT_TAG v5.0.0
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
  EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(ftxui)

# -- Sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/backend/*.cpp)
file(GLOB_RECURSE TUI_SOURCES CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/tui/*.cpp)
file(GLOB_RECURSE SIMULATION CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/simulation/*.cpp)

# -- Binaries
add_executable(reactor-backend ${SOURCES})
add_executable(reactor-tui ${TUI_SOURCES})
add_executable(reactor-simulation ${SIMULATION})

# -- Targetting
target_include_directories(
  reactor-backend PRIVATE ${CMAKE_SOURCE_DIR}/includes/backend
                          ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include)
target_include_directories(reactor-tui PRIVATE ${CMAKE_SOURCE_DIR}/includes/tui)
target_include_directories(reactor-simulation
                           PRIVATE ${CMAKE_SOURCE_DIR}/includes/simulation)

include_directories(${CMAKE_SOURCE_DIR}/includes/common)

# Link FXTUI lib
target_link_libraries(
  reactor-tui
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component)

target_include_directories(reactor-tui PRIVATE ${ftxui_SOURCE_DIR}/include)
