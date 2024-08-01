#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Define a handle for the semaphore
SemaphoreHandle_t binarySemaphore;

// Task function declarations
void Task1(void *pvParameters);
void Task2(void *pvParameters);

int main(void) {
    // Initialize hardware (e.g., UART, peripherals, etc.)

    // Create the binary semaphore
    binarySemaphore = xSemaphoreCreateBinary();

    if (binarySemaphore == NULL) {
        // Handle error: Semaphore creation failed
        while (1);
    }

    // Create tasks
    xTaskCreate(Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    // If all goes well, we should never get here
    while (1);
}

void Task1(void *pvParameters) {
    for (;;) {
        // Wait for the semaphore to become available
        if (xSemaphoreTake(binarySemaphore, portMAX_DELAY) == pdTRUE) {
            // Semaphore acquired
            // Perform critical section tasks

            // Simulate some work
            vTaskDelay(pdMS_TO_TICKS(1000));

            // Release the semaphore
            xSemaphoreGive(binarySemaphore);
        }

        // Simulate task delay
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void Task2(void *pvParameters) {
    for (;;) {
        // Simulate some work
        vTaskDelay(pdMS_TO_TICKS(500));

        // Give the semaphore to Task1
        xSemaphoreGive(binarySemaphore);
        
        // Simulate task delay
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
