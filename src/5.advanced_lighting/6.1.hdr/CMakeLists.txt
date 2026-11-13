
cmake_minimum_required(VERSION 3.16)

project(5.6.1.hdr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(5.6.1.hdr 6.1.hdr.cpp)

target_link_libraries(5.6.1.hdr
    PRIVATE
        glad
        glfw
        stb_image
        glm
)

set(SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/6.1.hdr.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/6.1.hdr.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/6.1.lighting.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/6.1.lighting.fs"
)

set(TARGET_OUTPUT_DIR "$<TARGET_FILE_DIR:5.6.1.hdr>/")

add_custom_command(
    TARGET 5.6.1.hdr POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_FILES}
        ${TARGET_OUTPUT_DIR}
    COMMENT "Copying shader files to output directory..."
)

