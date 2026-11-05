cmake_minimum_required(VERSION 3.16)

project(5.1.advanced_lighting LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(5.1.advanced_lighting advanced_lighting.cpp)

target_link_libraries(5.1.advanced_lighting
    PRIVATE
        glad
        glfw
        stb_image
        glm
)

set(SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/1.advanced_lighting.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/1.advanced_lighting.fs"
)

set(TARGET_OUTPUT_DIR "$<TARGET_FILE_DIR:5.1.advanced_lighting>")

add_custom_command(
    TARGET 5.1.advanced_lighting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${SHADER_FILES}
        ${TARGET_OUTPUT_DIR}
    COMMENT "Copying shader files to output directory..."
)
