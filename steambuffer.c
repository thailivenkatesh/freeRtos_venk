#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

// Define buffer sizes
#define BUFFER_SIZE 100

// Define a handle for the stream buffer
StreamBufferHandle_t xStreamBuffer;

// Task function prototypes
void vTaskProducer(void *pvParameters);
void vTaskConsumer(void *pvParameters);

int main(void) {
    // Initialize hardware (e.g., UART, peripherals, etc.)

    // Create a stream buffer with a buffer size of BUFFER_SIZE
    xStreamBuffer = xStreamBufferCreate(BUFFER_SIZE, 1);

    if (xStreamBuffer == NULL) {
        // Handle error: Stream buffer creation failed
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
    char data[] = "Hello, FreeRTOS Stream Buffer!";
    size_t dataLength = sizeof(data);
    int errorvalue = 0;

    for (;;) {
        // Send data to the stream buffer
        size_t bytesWritten = xStreamBufferSend(xStreamBuffer, data, dataLength, portMAX_DELAY);

        if (bytesWritten != dataLength) {
            // Handle error: Failed to send all data
        	errorvalue = 1;
        }

        // Simulate additional work
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void vTaskConsumer(void *pvParameters) {
    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    int errorvalue = 0;

    for (;;) {
        // Receive data from the stream buffer
        bytesRead = xStreamBufferReceive(xStreamBuffer, buffer, sizeof(buffer), portMAX_DELAY);

        if (bytesRead > 0) {
            // Process the received data
            buffer[bytesRead] = '\0'; // Null-terminate the string
            // Example: Process or print the data
        } else {
            // Handle error: Failed to receive data or timeout occurred
        	errorvalue = 1;
        }

        // Simulate additional work
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
