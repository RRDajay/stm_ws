#include "main.h"
#include "usart.h"
#include "gpio.h"

#include "FreeRTOS.h"
#include "task.h"


/**
 * @brief Heartbeat task to indicate if system is still alive
 * 
 * @param param 
 */
static void heartbeatTask(void* param)
{
  for(;;)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

int app_main()
{
  BaseType_t result = xTaskCreate(heartbeatTask, "heartBeatTask", 128, NULL, 1, NULL);
  vTaskStartScheduler();
}