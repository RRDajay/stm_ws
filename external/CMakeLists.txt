set(STM32CUBEMX_GENERATED_FILES
    stmcubemx/Core/Src/gpio.c
    stmcubemx/Core/Src/main.c
    stmcubemx/Core/Src/stm32f1xx_hal_msp.c
    stmcubemx/Core/Src/stm32f1xx_hal_timebase_tim.c
    stmcubemx/Core/Src/stm32f1xx_it.c
    stmcubemx/Core/Src/syscalls.c
    stmcubemx/Core/Src/sysmem.c
    stmcubemx/Core/Src/system_stm32f1xx.c
    stmcubemx/Core/Src/usart.c
    
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    
    # Startup code in assembly
    stmcubemx/Core/Startup/startup_stm32f103c8tx.s)
set(STMCUBE32MX_GENERATED_CODE stm32generatedcode)

add_library(${STMCUBE32MX_GENERATED_CODE} ${STM32CUBEMX_GENERATED_FILES})
target_compile_definitions(${STMCUBE32MX_GENERATED_CODE} PRIVATE 
    -DUSE_HAL_DRIVER 
    -DSTM32F103xB)

target_compile_options(${STMCUBE32MX_GENERATED_CODE} PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -fdata-sections
    -ffunction-sections
    -Wall
    $<$<CONFIG:Debug>:-Og -gdwarf-2>)
    
target_include_directories(${STMCUBE32MX_GENERATED_CODE} PRIVATE 
    stmcubemx/Core/Inc
    stmcubemx/Core/Inc
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Inc
    stmcubemx/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    stmcubemx/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    stmcubemx/Drivers/CMSIS/Include
    stmcubemx/FreeRTOS-Kernel/include
    stmcubemx/FreeRTOS-Kernel/portable/GCC/ARM_CM3)