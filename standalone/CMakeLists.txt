cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(ChessStandalone
  LANGUAGES CXX
)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(
  NAME cxxopts
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 2.2.0
  OPTIONS
    "CXXOPTS_BUILD_EXAMPLES Off"
    "CXXOPTS_BUILD_TESTS Off"
)

CPMAddPackage(
  NAME chess
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..
)

# ---- Create standalone executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(ChessStandalone ${sources})

set_target_properties(ChessStandalone PROPERTIES 
  CXX_STANDARD 17 
  OUTPUT_NAME "chess"
)

target_link_libraries(ChessStandalone chess cxxopts)
