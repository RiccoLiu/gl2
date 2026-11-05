cmake_minimum_required(VERSION 3.16)

project(assimp LANGUAGES C CXX) 

# 导入预编译的头文件和动态库assimp，并将它设为全局库
add_library(assimp SHARED IMPORTED GLOBAL)

set (ASSIMP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set (ASSIMP_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-vc143-mt.lib")
set (ASSIMP_DLL "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-vc143-mt.dll")
set (ASSIMP_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/) # dll 库输出路径

set_target_properties(assimp PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${ASSIMP_INCLUDE_DIR}
    IMPORTED_IMPLIB ${ASSIMP_LIBRARY}
    IMPORTED_LOCATION ${ASSIMP_DLL} 
)

add_custom_target(copy_assimp_dll ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSIMP_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSIMP_DLL} ${ASSIMP_OUTPUT_PATH}
    COMMENT "Copying ${ASSIMP_DLL} to ${ASSIMP_OUTPUT_PATH} directory..."
)

file(GLOB ASSIMP_DEP_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/dll/*.dll")
if(ASSIMP_DEP_DLLS)
    add_custom_target(copy_assimp_dep_dll ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSIMP_OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASSIMP_DEP_DLLS} ${ASSIMP_OUTPUT_PATH}
        COMMENT "Copying ${ASSIMP_DEP_DLLS} to ${ASSIMP_OUTPUT_PATH} directory..."
    )
endif()

# 导入动态运行库
function(target_copy_assimp_dll TARGET_NAME)
    get_target_property(ASSIMP_DLL_PATH assimp IMPORTED_LOCATION)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ASSIMP_DLL_PATH}"
        $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMENT "Copying ${ASSIMP_DLL_PATH} to ${TARGET_NAME} output directory..."
    )
endfunction()



