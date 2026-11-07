
cmake_minimum_required(VERSION 3.16)

project(5.3.1.shadow_mapping_depth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(5.3.1.shadow_mapping_depth 3.1.shadow_mapping_depth.cpp)

target_link_libraries(5.3.1.shadow_mapping_depth
    PRIVATE
        glad
        glfw
        stb_image
        glm
)

# set_target_properties(5.3.1.shadow_mapping_depth PROPERTIES
#     VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:5.3.1.shadow_mapping_depth>/"
# )

set(SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/3.1.shadow_mapping_depth.vs" 
    "${CMAKE_CURRENT_SOURCE_DIR}/3.1.shadow_mapping_depth.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/3.1.debug_quad.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/3.1.debug_quad_depth.fs"
    "${CMAKE_CURRENT_SOURCE_DIR}/3.1.shadow_mapping.vs"
    "${CMAKE_CURRENT_SOURCE_DIR}/3.1.shadow_mapping.fs"
)

set(TARGET_OUTPUT_DIR "$<TARGET_FILE_DIR:5.3.1.shadow_mapping_depth>/")


add_custom_command(
    TARGET 5.3.1.shadow_mapping_depth POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER_FILES}
        ${TARGET_OUTPUT_DIR}
    COMMENT "Copying shader files to output directory..."
)

