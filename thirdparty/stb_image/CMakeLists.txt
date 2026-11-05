
cmake_minimum_required(VERSION 3.16)

project(stb_image LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(stb_image STATIC stb_image.cpp)

target_include_directories(stb_image
  PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
)
