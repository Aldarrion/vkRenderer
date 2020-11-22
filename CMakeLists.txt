cmake_minimum_required(VERSION 3.14)

set(PROJ_NAME VkRenderer)
project(${PROJ_NAME})

add_subdirectory(Engine)

set(CMAKE_CXX_STANDARD 17)

# Vkr setup
file(GLOB_RECURSE VKR_HEADERS "Vkr/include/*.h")
file(GLOB_RECURSE VKR_SOURCES "Vkr/src/*.cpp")
file(GLOB_RECURSE EDITORCONFIG "Engine/.editorconfig")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Vkr/include" PREFIX "Vkr" FILES ${VKR_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Vkr/src" PREFIX "Vkr" FILES ${VKR_SOURCES})

add_executable(${PROJ_NAME} WIN32 ${VKR_SOURCES} ${VKR_HEADERS} ${EDITORCONFIG})

target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Vkr/include")

target_link_libraries(${PROJ_NAME} HiddenEngine)

set_property(TARGET ${PROJ_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Data")

#
include(Engine/CMakeCommon.cmake)
SetupCompiler(${PROJ_NAME})

# Solution name
project (VkRenderer)
