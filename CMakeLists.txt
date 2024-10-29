# /*

#  Author: Yannis STEFANELLI

#  Creation Date: 20-06-2024 11:01:49

#  Description :
#    CmakeLists to generate .dll and .lib files for the project
# */

cmake_minimum_required(VERSION 3.26)

project(steam_wrapper CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(
  GLOB SOURCES CONFIGURE_DEPENDS
  "src/*.cpp"
)

if(NOT DEFINED STEAM_FOLDER)
  message("\nYou have to specifiy the Steam SDK folder location : -DSTEAM_FOLDER=\"relativePath\"\n")
endif()

set(CMAKE_CURRENT_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${STEAM_FOLDER})

#display the include directories
message(STATUS "Include directories: ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_CURRENT_LIB_DIR}/include")
#display the link directories
message(STATUS "Link directories: ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_CURRENT_LIB_DIR}/lib")
include_directories(${CMAKE_CURRENT_LIB_DIR}/include)
link_directories(${CMAKE_CURRENT_LIB_DIR}/lib)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(
    -static-libgcc
    -static-libstdc++
    #  -g3
    -Wall
    -Wextra
    -Werror
    -static
    -O3
    )
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "LLVM" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(
      -mllvm
      -disable-copyprop
  )
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCES})
add_library(${PROJECT_NAME}_static STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} steam_api64)
