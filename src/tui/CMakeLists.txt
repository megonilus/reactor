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

# -- TUI Sources
file(GLOB_RECURSE TUI_SOURCES CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/tui/*.cpp)

# -- Library
add_library(reactor-tui STATIC ${TUI_SOURCES})

# -- Targets
target_include_directories(reactor-tui PUBLIC ${CMAKE_SOURCE_DIR}/includes/tui)
target_include_directories(reactor-tui PRIVATE ${ftxui_SOURCE_DIR}/include)

# -- Link FXTUI lib
target_link_libraries(
  reactor-tui
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component)

# -- Compile definitions. For displaying app information in TUI
target_compile_definitions(
  reactor-tui PRIVATE PROJECT_NAME="${PROJECT_NAME}"
                      PROJECT_VERSION="${PROJECT_VERSION}")
