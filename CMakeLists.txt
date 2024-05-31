cmake_minimum_required(VERSION 3.10.0)
#####################################################################
# Setup the toolchain                                               #
#####################################################################
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/arm-gcc-toolchain.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL ON FORCE)

project(stm32f1xx)
enable_language(C CXX ASM)
#####################################################################
# Build freeRTOS kernel as a static library                         #
#####################################################################
add_library(freertos_config INTERFACE)
set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)
# Select the native compile PORT
set(FREERTOS_PORT GCC_ARM_CM3 CACHE STRING "")

target_include_directories(freertos_config SYSTEM
    INTERFACE external/stmcubemx/Core/Inc)
target_compile_options(freertos_config INTERFACE
    -mcpu=cortex-m3
    -mthumb
    -fdata-sections
    -ffunction-sections
    -Wall
    $<$<CONFIG:Debug>:-Og -gdwarf-2>)
add_subdirectory(external/FreeRTOS-Kernel)
#####################################################################
# Build the firmware                                                #
#####################################################################
set(STMCUBE32MX_GENERATED_CODE stm32generatedcode)
set(APPLICATION_FILES
    app/app_main.cpp)
    
# Build the main target application code
add_subdirectory(external)
add_executable(${PROJECT_NAME} ${APPLICATION_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    -DUSE_HAL_DRIVER 
    -DSTM32F103xB)

target_compile_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -fdata-sections
    -ffunction-sections
    -Wall
    $<$<CONFIG:Debug>:-Og -gdwarf-2>)

target_include_directories(${PROJECT_NAME} PRIVATE 
    external/stmcubemx/Core/Inc
    external/stmcubemx/Drivers/STM32F1xx_HAL_Driver/Inc
    external/stmcubemx/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    external/stmcubemx/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    external/stmcubemx/Drivers/CMSIS/Include
    FreeRTOS-Kernel/include
    FreeRTOS-Kernel/portable/GCC/ARM_CM3)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE freertos_kernel ${STMCUBE32MX_GENERATED_CODE})

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/external/stmcubemx/STM32F103C8TX_FLASH.ld
    -mcpu=cortex-m3
    -mthumb
    # -specs=nano.specs
    -specs=nosys.specs
    -lc
    -lm
    # -lnosys
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections)

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