cmake_minimum_required(VERSION 3.10.0)
project(gl2 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(MSVC)
    add_compile_options(/utf-8)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# link_directories(${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/glfw-3.5.1.bin.WIN64)
add_subdirectory(thirdparty/stb_image)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/assimp)
add_subdirectory(src/1.getting_started/2.1.hello_triangle)
add_subdirectory(src/3.model_loading/1.model_loading)
add_subdirectory(src/5.advanced_lighting/1.advanced_lighting)
add_subdirectory(src/5.advanced_lighting/2.gamma_correction)
add_subdirectory(src/5.advanced_lighting/3.1.shadow_mapping_depth)
add_subdirectory(src/5.advanced_lighting/3.2.point_shadows)
add_subdirectory(src/5.advanced_lighting/4.1.normal_mapping)
add_subdirectory(src/5.advanced_lighting/6.1.hdr)
add_subdirectory(src/5.advanced_lighting/7.1.bloom)
add_subdirectory(src/5.advanced_lighting/8.1.defered_shading)

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/resources
    COMMENT "Copying resources to output directory..."
)
