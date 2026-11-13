
cmake_minimum_required(VERSION 3.16)

project(5.3.2.point_shadows LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(5.3.2.point_shadows 3.2.point_shadows.cpp)

target_link_libraries(5.3.2.point_shadows
    PRIVATE
        glad
        glfw
        stb_image
        glm
)

set(SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/3.2.point_shadows_depth.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/3.2.point_shadows_depth.gs"
    "${CMAKE_CURRENT_SOURCE_DIR}/3.2.point_shadows_depth.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/3.2.point_shadows.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/3.2.point_shadows.fs"
)

set(TARGET_OUTPUT_DIR "$<TARGET_FILE_DIR:5.3.2.point_shadows>/")

add_custom_command(
    TARGET 5.3.2.point_shadows POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_FILES}
        ${TARGET_OUTPUT_DIR}
    COMMENT "Copying shader files to output directory..."
)

