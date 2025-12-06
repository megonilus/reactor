# -- Create reactor-config lib
add_library(reactor-config STATIC config.cpp)

# -- Target needed include directories
target_include_directories(
  reactor-config PRIVATE ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include
                         ${CMAKE_SOURCE_DIR}/includes/config)
