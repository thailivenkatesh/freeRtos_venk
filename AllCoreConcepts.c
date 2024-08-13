#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "main.h"

// Definitions for event group bits
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)

// Queue handles
QueueHandle_t xQueue;
QueueSetHandle_t xQueueSet;

// Semaphore handles
SemaphoreHandle_t xBinarySemaphore;
SemaphoreHandle_t xCountingSemaphore;
SemaphoreHandle_t xMutex;

// Event group handle
EventGroupHandle_t xEventGroup;

// Stream buffer and message buffer handles
StreamBufferHandle_t xStreamBuffer;
MessageBufferHandle_t xMessageBuffer;

// Timer handle
TimerHandle_t xTimer;

// Task notifications
TaskHandle_t xTask1Handle, xTask2Handle, xTask3Handle, xTask4Handle;
TaskHandle_t xHighPriorityTaskHandle, xMediumPriorityTaskHandle, xLowPriorityTaskHandle;

// Task declarations
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);
void vTask4(void *pvParameters);
void vHighPriorityTask(void *pvParameters);
void vMediumPriorityTask(void *pvParameters);
void vLowPriorityTask(void *pvParameters);

// Timer callback function
void vTimerCallback(TimerHandle_t xTimer);

// Interrupt handler declaration
void EXTI15_10_IRQHandler(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void button_init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

int main(void) {
	// Initialize hardware (e.g., UART, peripherals, etc.)
	button_init();

	// Create Queue
	xQueue = xQueueCreate(10, sizeof(int));

	// Create Queue Set
	xQueueSet = xQueueCreateSet(10);

	// Add queue to queue set
	xQueueAddToSet(xQueue, xQueueSet);

	// Create Binary Semaphore
	xBinarySemaphore = xSemaphoreCreateBinary();

	// Create Counting Semaphore with a max count of 5 and initial count of 0
	xCountingSemaphore = xSemaphoreCreateCounting(5, 0);

	// Create Mutex
	xMutex = xSemaphoreCreateMutex();

	// Create Event Group
	xEventGroup = xEventGroupCreate();

	// Create Stream Buffer
	xStreamBuffer = xStreamBufferCreate(100, 10);

	// Create Message Buffer
	xMessageBuffer = xMessageBufferCreate(100);

	// Create Timer
	xTimer = xTimerCreate("Timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, vTimerCallback);

	// Start the timer
	xTimerStart(xTimer, 0);

	// Create tasks
	xTaskCreate(vTask1, "Task 1", 1000, NULL, 1, &xTask1Handle);
	xTaskCreate(vTask2, "Task 2", 1000, NULL, 1, &xTask2Handle);
	xTaskCreate(vTask3, "Task 3", 1000, NULL, 1, &xTask3Handle);
	xTaskCreate(vTask4, "Task 4", 1000, NULL, 1, &xTask4Handle);

	// Create high, medium, and low priority tasks
	xTaskCreate(vHighPriorityTask, "High Priority Task", 1000, NULL, 3, &xHighPriorityTaskHandle);
	xTaskCreate(vMediumPriorityTask, "Medium Priority Task", 1000, NULL, 2, &xMediumPriorityTaskHandle);
	xTaskCreate(vLowPriorityTask, "Low Priority Task", 1000, NULL, 1, &xLowPriorityTaskHandle);

	// Start the scheduler
	vTaskStartScheduler();

	// Infinite loop
	while (1) {}
}

void vTask1(void *pvParameters) {
	char dataTx[20] = {0};

	while (1) {
		// Prepare a string message
		sprintf(dataTx,  "MessageNormal %d",sizeof(dataTx));
		// Send data to queue
		xQueueSend(xQueue, &dataTx, portMAX_DELAY);

		// Signal binary semaphore
		xSemaphoreGive(xBinarySemaphore);

		// Increment counting semaphore
		xSemaphoreGive(xCountingSemaphore);

		// Set event group bits
		xEventGroupSetBits(xEventGroup, BIT_0);

		// Write to stream buffer
		xStreamBufferSend(xStreamBuffer, &dataTx, sizeof(dataTx), portMAX_DELAY);

		// Write to message buffer
		xMessageBufferSend(xMessageBuffer, &dataTx, sizeof(dataTx), portMAX_DELAY);

		// Notify task 2
		xTaskNotifyGive(xTask2Handle);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void vTask2(void *pvParameters) {
	char dataRx[20];
	while (1) {
		// Wait for task notification from task 1
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Wait for data from queue
		xQueueReceive(xQueue, &dataRx, portMAX_DELAY);

		// Wait for binary semaphore
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

		// Wait for counting semaphore
		xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);

		// Wait for event group bits
		xEventGroupWaitBits(xEventGroup, BIT_0, pdTRUE, pdFALSE, portMAX_DELAY);

		// Receive from stream buffer
		xStreamBufferReceive(xStreamBuffer, &dataRx, sizeof(dataRx), portMAX_DELAY);

		// Receive from message buffer
		xMessageBufferReceive(xMessageBuffer, &dataRx, sizeof(dataRx), portMAX_DELAY);

		// Enter critical section
		taskENTER_CRITICAL();
		printf("Task 2: Entered Critical Section\n");

		// Perform operations that should not be interrupted
		// Critical section logic here

		// Exit critical section
		taskEXIT_CRITICAL();
		printf("Task 2: Exited Critical Section\n");

		// Lock mutex
		if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
			printf("Task 2: Mutex locked\n");

			// Critical section - thread-safe operation
			// Perform operations that need to be thread-safe

			// Unlock mutex
			xSemaphoreGive(xMutex);
			printf("Task 2: Mutex unlocked\n");
		}

		// Notify task 3
		xTaskNotifyGive(xTask3Handle);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void vTask3(void *pvParameters) {
	while (1) {
		// Wait for task notification from task 2
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Enter critical section
		taskENTER_CRITICAL();
		printf("Task 3: Entered Critical Section\n");

		// Perform operations that should not be interrupted
		// Critical section logic here

		// Exit critical section
		taskEXIT_CRITICAL();
		printf("Task 3: Exited Critical Section\n");

		// Wait for an event from queue set
		QueueSetMemberHandle_t xActivatedMember = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);
		if (xActivatedMember == xQueue) {
			// Handle queue event
		}

		// Set event group bits
		xEventGroupSetBits(xEventGroup, BIT_1);

		// Notify task 4
		xTaskNotifyGive(xTask4Handle);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void vTask4(void *pvParameters) {
	while (1) {
		// Wait for task notification from task 3
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Wait for event group bits
		xEventGroupWaitBits(xEventGroup, BIT_1, pdTRUE, pdFALSE, portMAX_DELAY);

		// Set event group bits
		xEventGroupSetBits(xEventGroup, BIT_2);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void vHighPriorityTask(void *pvParameters) {
	while (1) {
		// High-priority task does some critical work
		// It could pre-empt other tasks due to its priority
		xEventGroupSetBits(xEventGroup, BIT_3);
		printf("High Priority Task Running\n");

		// Notify Medium Priority Task
		xTaskNotifyGive(xMediumPriorityTaskHandle);

		vTaskDelay(pdMS_TO_TICKS(500)); // Runs every 500 ms
	}
}

void vMediumPriorityTask(void *pvParameters) {
	while (1) {
		// Wait for task notification from High Priority Task
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Perform some medium-priority task
		printf("Medium Priority Task Running\n");

		// Notify Low Priority Task
		xTaskNotifyGive(xLowPriorityTaskHandle);

		vTaskDelay(pdMS_TO_TICKS(1000)); // Runs every 1000 ms
	}
}

void vLowPriorityTask(void *pvParameters) {
	while (1) {
		// Wait for task notification from Medium Priority Task
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Perform some low-priority task
		printf("Low Priority Task Running\n");

		vTaskDelay(pdMS_TO_TICKS(2000)); // Runs every 2000 ms
	}
}

void vTimerCallback(TimerHandle_t xTimer) {
	// Timer callback actions
	// Set event group bits
	xEventGroupSetBits(xEventGroup, BIT_2);
}

void EXTI15_10_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	char dataIntTx[20] = {0};
	sprintf(dataIntTx, "fromInterrupt %d",sizeof(dataIntTx));

	if (GPIO_Pin == GPIO_PIN_13) {
		// ISR: Signal binary semaphore from ISR
		xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);

		// ISR: Increment counting semaphore from ISR
		xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);

		// ISR: Set event group bits from ISR
		xEventGroupSetBitsFromISR(xEventGroup, BIT_0, &xHigherPriorityTaskWoken);

		// ISR: Send data to queue from ISR
		xQueueSendFromISR(xQueue, &dataIntTx, &xHigherPriorityTaskWoken);

		// ISR: Write to stream buffer from ISR
		xStreamBufferSendFromISR(xStreamBuffer, &dataIntTx, sizeof(dataIntTx), &xHigherPriorityTaskWoken);

		// ISR: Write to message buffer from ISR
		xMessageBufferSendFromISR(xMessageBuffer, &dataIntTx, sizeof(dataIntTx), &xHigherPriorityTaskWoken);

		// ISR: Notify task 1
		vTaskNotifyGiveFromISR(xTask1Handle, &xHigherPriorityTaskWoken);

		// If xHigherPriorityTaskWoken was set to true, yield to ensure proper task switching
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
