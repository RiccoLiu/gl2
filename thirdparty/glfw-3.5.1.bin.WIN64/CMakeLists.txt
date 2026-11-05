cmake_minimum_required(VERSION 3.16)

project(glfw LANGUAGES C CXX) 

# 导入预编译的头文件和动态库glfw，并将它设为全局库
add_library(glfw SHARED IMPORTED GLOBAL)

set (GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set (GLFW_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib-vc2022/glfw3.lib")
set (GLFW_DLL "${CMAKE_CURRENT_SOURCE_DIR}/lib-vc2022/glfw3.dll")
set (GLFW_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/) # dll 库输出路径

set_target_properties(glfw PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GLFW_INCLUDE_DIR}
    IMPORTED_IMPLIB ${GLFW_LIBRARY}
    IMPORTED_LOCATION ${GLFW_DLL}
)

add_custom_target(copy_glfw_dll ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GLFW_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${GLFW_DLL}
        ${GLFW_OUTPUT_PATH}
    COMMENT "Copying ${GLFW_DLL} to ${GLFW_OUTPUT_PATH} directory..."
)

# 对外支持手动导入动态运行库
function(target_copy_glfw_dll TARGET_NAME)
    get_target_property(GLFW_DLL_PATH glfw IMPORTED_LOCATION)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_DLL_PATH}"
        $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying ${GLFW_DLL_PATH} to ${TARGET_NAME} output directory..."
    )
endfunction()

