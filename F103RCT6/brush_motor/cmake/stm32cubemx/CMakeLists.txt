cmake_minimum_required(VERSION 3.22)

project(stm32cubemx)
add_library(stm32cubemx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32cubemx INTERFACE
	USE_HAL_DRIVER
	STM32F103xE
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32cubemx INTERFACE
    ../../Core/Inc
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Inc
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/CMSIS/Include
)

target_sources(stm32cubemx INTERFACE
    # ../../Core/Src/main.c
    # ../../Core/Src/gpio.c
    # ../../Core/Src/can.c
    # ../../Core/Src/tim.c
    # ../../Core/Src/stm32f1xx_it.c
    # ../../Core/Src/stm32f1xx_hal_msp.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.5/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
    # ../../Core/Src/system_stm32f1xx.c
    # ../../Core/Src/sysmem.c
    # ../../Core/Src/syscalls.c
    ../../startup_stm32f103xe.s
)

target_link_directories(stm32cubemx INTERFACE
)

target_link_libraries(stm32cubemx INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


