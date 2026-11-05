

cmake_minimum_required(VERSION 3.16)
project(glm LANGUAGES C CXX) 

# 导入纯头文件的库: glm
add_library(glm INTERFACE)

target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
