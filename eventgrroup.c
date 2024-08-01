#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

// Define bit positions for event flags
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)

// Define a handle for the event group
EventGroupHandle_t xEventGroup;

// Task function prototypes
void vTaskProducer(void *pvParameters);
void vTaskConsumer(void *pvParameters);

int main(void) {
    // Initialize hardware (e.g., UART, peripherals, etc.)

    // Create the event group
    xEventGroup = xEventGroupCreate();

    if (xEventGroup == NULL) {
        // Handle error: Event group creation failed
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

        // Simulate additional work
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
