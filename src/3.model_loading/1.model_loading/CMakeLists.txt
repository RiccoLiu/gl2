cmake_minimum_required(VERSION 3.16)

project(3.1.model_loading LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(3.1.model_loading model_loading.cpp)

target_link_libraries(3.1.model_loading
    PRIVATE
        glad
        glfw
        stb_image
        glm
        assimp
)

set(VERTEX_SHADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/1.model_loading.vs")
set(FRAGMENT_SHADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/1.model_loading.fs")
set(TARGET_OUTPUT_DIR "$<TARGET_FILE_DIR:3.1.model_loading>")

add_custom_command(
    TARGET 3.1.model_loading POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${VERTEX_SHADER_FILE}
        ${FRAGMENT_SHADER_FILE}
        ${TARGET_OUTPUT_DIR}
    COMMENT "Copying shader files to output directory..."
)
