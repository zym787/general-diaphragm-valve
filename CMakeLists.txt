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
set(CMAKE_PROJECT_NAME general-column-valve)

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
#     Kernal/loop.c
#     Kernal/tx_initialize_low_level.S
    Kernal/tx_initialize_low_level.S
    Bsp/bsp.c
    Bsp/Src/bsp_dwt.c
    Bsp/Src/bsp_timer.c
    Bsp/Src/bsp_led.c
    Bsp/Src/bsp_log.c
    Bsp/bsp_msg.c
    Bsp/Src/bsp_uart_fifo.c
    Bsp/Src/bsp_user_lib.c
    Bsp/Src/bsp_modbus.c
    Bsp/Src/bsp_motor.c
   
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
#     Kernal
        Bsp
        Bsp/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    TX_INCLUDE_USER_DEFINE_FILE
    THREADX_ENABLED
    DEBUG_BSP=1
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# message("source dir: " ${CMAKE_SOURCE_DIR})

#################################################### Add Project Manager Support
include("cmake/custom_commands.cmake")

# 根据自己添加的代码目录修改
# 头文件使用add_header_dirs函数添加，源文件使用add_source_files函数添加
add_header_dirs(${CMAKE_SOURCE_DIR}/Kernal)
add_source_files(${CMAKE_SOURCE_DIR}/Kernal)

# threadx支援
# set(THREADX_ARCH cortex_m3)
# set(THREADX_TOOLCHAIN gnu)
# set(TX_USER_FILE "${CMAKE_CURRENT_LIST_DIR}/UserCfg/tx_user.h")

add_header_dirs(${CMAKE_CURRENT_LIST_DIR}/Middlewares/ST/threadx/common/inc)
add_source_files(${CMAKE_CURRENT_LIST_DIR}/Middlewares/ST/threadx/common/src)
add_header_dirs(${CMAKE_CURRENT_LIST_DIR}/Middlewares/ST/threadx/ports/cortex_m3/gnu/inc)
add_source_files(${CMAKE_CURRENT_LIST_DIR}/Middlewares/ST/threadx/ports/cortex_m3/gnu/src)
# add_header_dirs(${CMAKE_CURRENT_LIST_DIR}/App)
# add_source_files(${CMAKE_CURRENT_LIST_DIR}/App)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Middlewares/ST/threadx)     # 添加子目录
# 添加threadx子模块存在性检查
if(EXISTS ${CMAKE_SOURCE_DIR}/Middlewares/ST/threadx)
        message("Threadx submodule found!")
else()
    message(FATAL_ERROR "Threadx submodule missing! Run: git submodule update --init")
endif()

# app支援
add_header_dirs(${CMAKE_CURRENT_LIST_DIR}/App)
add_source_files(${CMAKE_CURRENT_LIST_DIR}/App)

# bsp支援
# add_header_dirs(${CMAKE_CURRENT_LIST_DIR}/Bsp)
# add_source_files(${CMAKE_CURRENT_LIST_DIR}/Bsp)

# modbus支援
add_header_dirs(${CMAKE_CURRENT_LIST_DIR}/Middlewares/Third_Party/Armfly/modbus)
add_source_files(${CMAKE_CURRENT_LIST_DIR}/Middlewares/Third_Party/Armfly/modbus)

# Component支援
# stepper motor驱动支援
add_header_dirs(${CMAKE_CURRENT_LIST_DIR}/Component/motor)
add_source_files(${CMAKE_CURRENT_LIST_DIR}/Component/motor)
