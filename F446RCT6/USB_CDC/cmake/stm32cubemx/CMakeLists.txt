cmake_minimum_required(VERSION 3.22)

project(stm32cubemx)
add_library(stm32cubemx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32cubemx INTERFACE
	USE_HAL_DRIVER
	STM32F446xx
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32cubemx INTERFACE
    ../../Core/Inc
    ../../USB_DEVICE/App
    ../../USB_DEVICE/Target
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Inc
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/CMSIS/Include
)

target_sources(stm32cubemx INTERFACE
    ../../Core/Src/gpio.c
    ../../Core/Src/crc.c
    ../../Core/Src/dma.c
    ../../Core/Src/i2c.c
    ../../Core/Src/stm32f4xx_it.c
    ../../Core/Src/stm32f4xx_hal_msp.c
    ../../USB_DEVICE/App/usb_device.cpp
    ../../USB_DEVICE/App/usbd_desc.cpp
    ../../USB_DEVICE/App/usbd_cdc_if.cpp
    ../../USB_DEVICE/Target/usbd_conf.c
    ../../USB_DEVICE/App/usbd_cdc.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c
    ../../Core/Src/system_stm32f4xx.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    C:/Users/LC/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ../../Core/Src/sysmem.c
    ../../Core/Src/syscalls.c
    ../../startup_stm32f446xx.s
)

target_link_directories(stm32cubemx INTERFACE
)

target_link_libraries(stm32cubemx INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


