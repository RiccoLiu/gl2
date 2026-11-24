
cmake_minimum_required(VERSION 3.16)

project(6.1.1.lighting_textured LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(6.1.1.lighting_textured 1.1.lighting_textured.cpp)

target_link_libraries(6.1.1.lighting_textured
    PRIVATE
        glad
        glfw
        stb_image
        glm
        assimp
)

set(SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/1.1.pbr.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/1.1.pbr.fs"
)

set(TARGET_OUTPUT_DIR "$<TARGET_FILE_DIR:6.1.1.lighting_textured>/")

add_custom_command(
    TARGET 6.1.1.lighting_textured POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_FILES}
        ${TARGET_OUTPUT_DIR}
    COMMENT "Copying shader files to output directory..."
)

