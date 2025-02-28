cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME USB_CDC)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# ------------------------------------------------------------------------------------------------

# 生成 .hex 文件
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
    COMMENT "Creating ${PROJECT_NAME}.hex"
)

# 复制 .elf 文件
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_BINARY_DIR}/build.elf
    COMMENT "Copying ${CMAKE_PROJECT_NAME}.elf to build.elf for debugging"
)

# 定义下载目标
add_custom_target(download
    COMMAND openocd -f interface/cmsis-dap-swd.cfg -f target/stm32f4x.cfg -c "program ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex verify reset exit"
    DEPENDS ${PROJECT_NAME}
    COMMENT "Downloading ${PROJECT_NAME}.hex to target using OpenOCD"
)

# 查找所有的 .c、.cpp 和 .s 文件
file(GLOB_RECURSE ADDITIONAL_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.s"
)

# 查找所有包含 .h 和 .hpp 文件的目录
file(GLOB_RECURSE HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.hpp"
)
set(ADDITIONAL_INCLUDE_DIRS "")
foreach(HEADER ${HEADER_FILES})
    get_filename_component(DIR ${HEADER} PATH)
    list(APPEND ADDITIONAL_INCLUDE_DIRS ${DIR})
endforeach()
list(REMOVE_DUPLICATES ADDITIONAL_INCLUDE_DIRS)

# 打印找到的源文件和包含目录（可选，用于调试）
message(STATUS "Found additional source files: ${ADDITIONAL_SOURCE_FILES}")
message(STATUS "Found additional include directories: ${ADDITIONAL_INCLUDE_DIRS}")

# 将找到的源文件添加到目标
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${ADDITIONAL_SOURCE_FILES})

# 将找到的包含目录添加到目标
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${ADDITIONAL_INCLUDE_DIRS})