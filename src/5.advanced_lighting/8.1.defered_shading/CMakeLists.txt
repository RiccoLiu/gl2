
cmake_minimum_required(VERSION 3.16)

project(5.8.1.defered_shading LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(5.8.1.defered_shading 8.1.defered_shading.cpp)

target_link_libraries(5.8.1.defered_shading
    PRIVATE
        glad
        glfw
        stb_image
        glm
        assimp
)

set(SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/8.1.defered_shading.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/8.1.defered_shading.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/8.1.defered_light_box.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/8.1.defered_light_box.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/8.1.g_buffer.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/8.1.g_buffer.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/8.1.debug.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/8.1.debug.fs"
)

set(TARGET_OUTPUT_DIR "$<TARGET_FILE_DIR:5.8.1.defered_shading>/")

add_custom_command(
    TARGET 5.8.1.defered_shading POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_FILES}
        ${TARGET_OUTPUT_DIR}
    COMMENT "Copying shader files to output directory..."
)

