cmake_minimum_required(VERSION 3.14)

set(CMAKE_CONFIGURATION_TYPES "Release;Debug;RelWithDebInfo")

set(CMAKE_CXX_STANDARD 14)

set(PROJ_NAME VkRenderer)
project(${PROJ_NAME})

file(GLOB_RECURSE VKR_HEADERS "Render/include/*.h")
file(GLOB_RECURSE VKR_SOURCES "Render/src/*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Render/include" PREFIX "Render" FILES ${VKR_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Render/src" PREFIX "Render" FILES ${VKR_SOURCES})

add_executable(${PROJ_NAME} WIN32 ${VKR_SOURCES} ${VKR_HEADERS})

set(VK_SDK_DIR "" CACHE STRING "Root directory of the LunarG SDK")

target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/render/include")
target_include_directories(${PROJ_NAME} PRIVATE "${VK_SDK_DIR}/Include")

target_link_libraries(${PROJ_NAME} "${VK_SDK_DIR}/Lib/vulkan-1.lib")

if(MSVC)
    add_definitions(/MP)
    add_definitions(/D _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJ_NAME} PRIVATE /WX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4101") # Unreferenced local variable
endif()

# Solution name
project (VkRenderer)
