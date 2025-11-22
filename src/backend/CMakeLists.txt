# -- Get the sources
file(GLOB_RECURSE BACKEND_SOURCES CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/src/backend/*.cpp)

# -- Add library
add_library(reactor-backend STATIC ${BACKEND_SOURCES})

target_include_directories(
  reactor-backend
  PRIVATE ${CMAKE_SOURCE_DIR}/includes/backend
          ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include
          ${CMAKE_SOURCE_DIR}/includes/simulation)
