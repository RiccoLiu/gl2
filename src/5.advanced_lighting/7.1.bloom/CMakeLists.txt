
cmake_minimum_required(VERSION 3.16)

project(5.7.1.bloom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(5.7.1.bloom 7.1.bloom.cpp)

target_link_libraries(5.7.1.bloom
    PRIVATE
        glad
        glfw
        stb_image
        glm
)

set(SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/7.1.bloom.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/7.1.bloom.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/7.1.extract_hdr_bloom.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/7.1.extract_hdr_bloom.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/7.1.blur.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/7.1.blur.fs"
)

set(TARGET_OUTPUT_DIR "$<TARGET_FILE_DIR:5.7.1.bloom>/")

add_custom_command(
    TARGET 5.7.1.bloom POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_FILES}
        ${TARGET_OUTPUT_DIR}
    COMMENT "Copying shader files to output directory..."
)

