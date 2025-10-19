cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
#####################################################################
# Setup the toolchain                                               #
#####################################################################
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
if (NOT TX_USER_FILE)
    set(TX_USER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/app/os/inc/tx_user.h)
    message(STATUS "Using ${CMAKE_CURRENT_SOURCE_DIR}/app/os/inc/tx_user.h")
endif()
add_subdirectory(external/threadx)

#####################################################################
# Separate the Application, BSP, and OS specific files              #
#####################################################################
# Application specific files
set(APPLICATION_SPECIFIC_SOURCE_FILES
        app/main_app/src/main.c)

set(APPLICATION_SPECIFIC_INCLUDES
        app/main_app/inc)

# Vendor board specific files
set(BSP_SOURCE_FILES
        app/bsp/src/stm32h7xx_hal_msp.c
        app/bsp/src/stm32h7xx_hal_timebase_tim.c
        app/bsp/src/stm32h7xx_it.c
        app/bsp/src/syscalls.c
        app/bsp/src/sysmem.c
        app/bsp/src/system_stm32h7xx.c
        app/bsp/src/startup_stm32h750xbhx.s
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hsem.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c)

set(BSP_INCLUDES
        app/bsp/inc
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc
        external/STM32CubeH7/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
        external/STM32CubeH7/Drivers/CMSIS/Device/ST/STM32H7xx/Include
        external/STM32CubeH7/Drivers/CMSIS/Include)

set(BSP_DEFINES
        -DUSE_PWR_LDO_SUPPLY
        -DUSE_HAL_DRIVER 
        -DSTM32H750xx)

set(BSP_LINKER_FILE ${CMAKE_SOURCE_DIR}/STM32H750XBHX_FLASH.ld)

# OS specific files
set(OS_SOURCE_SPECIFIC_FILES
        app/os/src/app_threadx.c
        app/os/src/app_azure_rtos.c
        app/os/src/tx_initialize_low_level.S)

set(OS_SOURCE_SPECIFIC_INCLUDES
        app/os/inc)

#####################################################################
# Build the BSP as a static library                                 #
#####################################################################
add_library(bsp STATIC ${BSP_SOURCE_FILES})
target_include_directories(bsp PRIVATE ${BSP_INCLUDES})
target_compile_definitions(bsp PRIVATE ${BSP_DEFINES})
target_compile_options(bsp PRIVATE -ffunction-sections -fdata-sections -fstack-usage -Wall)

#####################################################################
# Build the firmware                                                #
#####################################################################
add_executable(${PROJECT_NAME} ${APPLICATION_SPECIFIC_SOURCE_FILES} ${OS_SOURCE_SPECIFIC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${APPLICATION_SPECIFIC_INCLUDES} ${BSP_INCLUDES} ${OS_SOURCE_SPECIFIC_INCLUDES})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${BSP_DEFINES})
target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections -fstack-usage -Wall)
target_link_options(${PROJECT_NAME} PRIVATE -T${BSP_LINKER_FILE} -lc -lm -Wl,-Map=${PROJECT_NAME}.map,--cref -Wl,--gc-sections)
target_link_libraries(${PROJECT_NAME} PRIVATE threadx bsp)

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