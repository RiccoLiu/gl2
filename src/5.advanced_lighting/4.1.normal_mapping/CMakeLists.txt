
cmake_minimum_required(VERSION 3.16)

project(5.4.1.normal_mapping LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(5.4.1.normal_mapping 4.1.normal_mapping.cpp)

target_link_libraries(5.4.1.normal_mapping
    PRIVATE
        glad
        glfw
        stb_image
        glm
)

set(SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/4.1.normal_mapping.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/4.1.normal_mapping.fs"
)

set(TARGET_OUTPUT_DIR "$<TARGET_FILE_DIR:5.4.1.normal_mapping>/")

add_custom_command(
    TARGET 5.4.1.normal_mapping POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_FILES}
        ${TARGET_OUTPUT_DIR}
    COMMENT "Copying shader files to output directory..."
)

