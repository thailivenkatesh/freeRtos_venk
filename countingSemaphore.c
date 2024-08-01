#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Define a handle for the counting semaphore
SemaphoreHandle_t countingSemaphore;

// Task function prototypes
void vTaskProducer(void *pvParameters);
void vTaskConsumer(void *pvParameters);

int main(void) {
    // Initialize hardware (e.g., UART, peripherals, etc.)

    // Create the counting semaphore with a maximum count of 5 and an initial count of 3
    countingSemaphore = xSemaphoreCreateCounting(5, 3);

    if (countingSemaphore == NULL) {
        // Handle error: Semaphore creation failed
        while (1); // Or appropriate error handling
    }

    // Create producer and consumer tasks
    xTaskCreate(vTaskProducer, "Producer", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vTaskConsumer, "Consumer", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    // Should not reach here if the scheduler is running correctly
    while (1);
}

void vTaskProducer(void *pvParameters) {
	int errorValue = 0;
	int count = 0;
    for (;;) {
        // Try to take the semaphore (decrement the count)
        if (xSemaphoreTake(countingSemaphore, portMAX_DELAY) == pdTRUE) {
            // Semaphore acquired
            // Perform critical section tasks
        	count = 1;
            // Simulate work by delaying
            vTaskDelay(pdMS_TO_TICKS(1000));

            // Release the semaphore (increment the count)
            xSemaphoreGive(countingSemaphore);
        } else {
            // Handle error: Failed to take the semaphore (shouldn't happen with portMAX_DELAY)
        	errorValue = 1;
        }

        // Simulate task delay
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void vTaskConsumer(void *pvParameters) {
    for (;;) {
        // Simulate some work
        vTaskDelay(pdMS_TO_TICKS(500));

        // Give the semaphore (increment the count)
        if (xSemaphoreGive(countingSemaphore) != pdTRUE) {
            // Handle error: Failed to give the semaphore
        }

        // Simulate task delay
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
