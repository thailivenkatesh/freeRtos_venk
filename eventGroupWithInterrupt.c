/*
 ******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2024-07-19

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 ******************************************************************************
 */

/* Includes */
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"

#include <stdio.h>
/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
// Define bit positions for event flags
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)

// Define a handle for the event group
EventGroupHandle_t xEventGroup;

TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;

// Task function prototypes
void vTaskProducer(void *pvParameters);
void vTaskConsumer(void *pvParameters);
/* MX_GPIO_Init() should be generated by CubeMX or manually defined */
void button_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn,10,0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


int main(void) {
	// Initialize hardware (e.g., UART, peripherals, etc.)
	button_init();
	// Create a stream buffer with a buffer size of BUFFER_SIZE
	  // Create the event group
	xEventGroup = xEventGroupCreate();

	if (xEventGroup == NULL) {
		// Handle error: Event group creation failed
		while (1); // Or appropriate error handling
	}

// Create producer and consumer tasks
	xTaskCreate(vTaskProducer, "Producer", configMINIMAL_STACK_SIZE, NULL, 1, &myTask1Handle);
	xTaskCreate(vTaskConsumer, "Consumer", configMINIMAL_STACK_SIZE, NULL, 1, &myTask2Handle);

	// Start the scheduler
	vTaskStartScheduler();

	// Should not reach here if the scheduler is running correctly
	while (1);
}

void vTaskProducer(void *pvParameters) {
    for (;;) {
        // Simulate some work
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Set bits in the event group
        xEventGroupSetBits(xEventGroup, BIT_0 | BIT_1);

        // Simulate additional work
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTaskConsumer(void *pvParameters) {
    EventBits_t uxBits;
    int count = 0;
    int errorvalue = 0;

    for (;;) {
        // Wait for any of the specified bits to be set (BIT_0 or BIT_1)
        uxBits = xEventGroupWaitBits(
            xEventGroup,           // Event group handle
            BIT_0 | BIT_1,        // Bits to wait for
            pdTRUE,                // Clear bits on exit
            pdFALSE,               // Wait for any bit
            portMAX_DELAY          // Wait indefinitely
        );

        // Check if the required bits were set
        if ((uxBits & (BIT_0 | BIT_1)) != 0) {
            // Perform actions based on the bits that were set
        	count = 1;
        } else {
            // Handle error: Unexpected bits were set
        	errorvalue = 1;
        }
        vTaskSuspend(NULL);
        // Simulate additional work
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_13)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xTaskResumeFromISR(myTask2Handle);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
