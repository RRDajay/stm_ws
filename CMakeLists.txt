cmake_minimum_required(VERSION 3.10.0)
#####################################################################
# Setup the toolchain                                               #
#####################################################################
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arm-gcc-toolchain.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL ON FORCE)

project(stm32h750b)
enable_language(C CXX ASM)

# Option to select RAM or FLASH linker script
option(LINK_TO_RAM "Link code to RAM for debugging" OFF)
set(LINKER_SCRIPT_FLASH "${CMAKE_SOURCE_DIR}/STM32H750XBHX_FLASH.ld")
set(LINKER_SCRIPT_RAM "${CMAKE_SOURCE_DIR}/STM32H750XBHX_RAM.ld")
if(LINK_TO_RAM)
    set(LINKER_SCRIPT ${LINKER_SCRIPT_RAM})
else()
    set(LINKER_SCRIPT ${LINKER_SCRIPT_FLASH})
endif()

#####################################################################
# Build freeRTOS kernel as a static library                         #
#####################################################################
# add_library(freertos_config INTERFACE)
# set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)
# set(FREERTOS_PORT GCC_ARM_CM4F CACHE STRING "")

# target_include_directories(freertos_config SYSTEM
#     INTERFACE Core/Inc)
# target_compile_options(freertos_config INTERFACE
#     -mcpu=cortex-m4
#     -mthumb
#     -mfpu=fpv4-sp-d16
#     -mfloat-abi=hard
#     -fdata-sections
#     -ffunction-sections
#     -Wall
#     $<$<CONFIG:Debug>:-Og -gdwarf-2>)
# add_subdirectory(FreeRTOS-Kernel)

#####################################################################
# Build ThreadX as a static library                                 #
#####################################################################
# add_subdirectory(threadx)

#####################################################################
# Build the firmware                                                #
#####################################################################
set(STM32CUBEMX_GENERATED_FILES

    Core/Src/main.c
    Core/Src/stm32h7xx_hal_msp.c
    Core/Src/stm32h7xx_hal_timebase_tim.c
    Core/Src/stm32h7xx_it.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    Core/Src/system_stm32h7xx.c

    # STM32 HAL drivers.
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hsem.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c
    Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c

    # Startup code
    Core/Startup/startup_stm32h750xbhx.s)

add_executable(${PROJECT_NAME} ${STM32CUBEMX_GENERATED_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        -DUSE_PWR_LDO_SUPPLY
        -DUSE_HAL_DRIVER 
        -DSTM32H750xx)

target_compile_options(${PROJECT_NAME} PRIVATE
    # -std=gnu11
    # -mcpu=cortex-m7
    # -mthumb
    # -mfpu=fpv5-d16
    # -mfloat-abi=hard
    # -march=armv7e-m
    # -ffunction-sections
    # -fdata-sections
    # -fstack-usage
    -Wall)

target_include_directories(${PROJECT_NAME} PRIVATE 
        Core/Inc
        Drivers/STM32H7xx_HAL_Driver/Inc
        Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32H7xx/Include
        Drivers/CMSIS/Include
        AZURE_RTOS/App
        )

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/STM32H750XBHX_FLASH.ld
    # -mcpu=cortex-m7
    # -mthumb
    # -mfpu=fpv5-d16
    # -mfloat-abi=hard
    # -march=armv7e-m
    # -lc 
    # -lm
    # -lnosys
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections)

# target_link_libraries(${PROJECT_NAME} PRIVATE threadx)

#####################################################################
# Add custom post-build commands                                    #
#####################################################################
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}>
    ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary -S $<TARGET_FILE:${PROJECT_NAME}>
    ${PROJECT_NAME}.bin)

add_custom_target(flash
    COMMAND openocd -f ${CMAKE_SOURCE_DIR}/stm32h750b.cfg -c "program ${PROJECT_NAME} verify reset exit"
    DEPENDS ${PROJECT_NAME}
    COMMENT "Flashing ${PROJECT_NAME} to STM32 using OpenOCD"
)