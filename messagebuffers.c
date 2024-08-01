#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

// Define buffer size
#define BUFFER_SIZE 100

// Define a handle for the message buffer
MessageBufferHandle_t xMessageBuffer;

// Task function prototypes
void vTaskSender(void *pvParameters);
void vTaskReceiver(void *pvParameters);

int main(void) {
    // Initialize hardware (e.g., UART, peripherals, etc.)

    // Create a message buffer with a size of BUFFER_SIZE
    xMessageBuffer = xMessageBufferCreate(BUFFER_SIZE);

    if (xMessageBuffer == NULL) {
        // Handle error: Message buffer creation failed
        while (1); // Or appropriate error handling
    }

    // Create sender and receiver tasks
    xTaskCreate(vTaskSender, "Sender", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vTaskReceiver, "Receiver", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    // Should not reach here if the scheduler is running correctly
    while (1);
}

void vTaskSender(void *pvParameters) {
    const char *pcMessage = "Hello, FreeRTOS Message Buffer!";
    size_t xMessageLength = strlen(pcMessage) + 1; // +1 for null terminator

    for (;;) {
        // Send a message to the message buffer
        size_t xBytesSent = xMessageBufferSend(xMessageBuffer, (void *)pcMessage, xMessageLength, portMAX_DELAY);

        if (xBytesSent != xMessageLength) {
            // Handle error: Failed to send the complete message
        }

        // Simulate additional work
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void vTaskReceiver(void *pvParameters) {
    char pcBuffer[BUFFER_SIZE];
    size_t xReceivedLength;

    for (;;) {
        // Receive a message from the message buffer
        xReceivedLength = xMessageBufferReceive(xMessageBuffer, pcBuffer, sizeof(pcBuffer), portMAX_DELAY);

        if (xReceivedLength > 0) {
            // Process the received message
            pcBuffer[xReceivedLength] = '\0'; // Null-terminate the string
            // Example: Print or use the message
        } else {
            // Handle error: Failed to receive the message or timeout occurred
        }

        // Simulate additional work
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

// Define buffer size
#define BUFFER_SIZE 100

// Define a handle for the message buffer
MessageBufferHandle_t xMessageBuffer;

// Task function prototypes
void vTaskSender(void *pvParameters);
void vTaskReceiver(void *pvParameters);

int main(void) {
    // Initialize hardware (e.g., UART, peripherals, etc.)

    // Create a message buffer with a size of BUFFER_SIZE
    xMessageBuffer = xMessageBufferCreate(BUFFER_SIZE);

    if (xMessageBuffer == NULL) {
        // Handle error: Message buffer creation failed
        while (1); // Or appropriate error handling
    }

    // Create sender and receiver tasks
    xTaskCreate(vTaskSender, "Sender", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vTaskReceiver, "Receiver", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    // Should not reach here if the scheduler is running correctly
    while (1);
}

void vTaskSender(void *pvParameters) {
    const char *pcMessage = "Hello, FreeRTOS Message Buffer!";
    size_t xMessageLength = strlen(pcMessage) + 1; // +1 for null terminator

    for (;;) {
        // Send a message to the message buffer
        size_t xBytesSent = xMessageBufferSend(xMessageBuffer, (void *)pcMessage, xMessageLength, portMAX_DELAY);

        if (xBytesSent != xMessageLength) {
            // Handle error: Failed to send the complete message
        }

        // Simulate additional work
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void vTaskReceiver(void *pvParameters) {
    char pcBuffer[BUFFER_SIZE];
    size_t xReceivedLength;

    for (;;) {
        // Receive a message from the message buffer
        xReceivedLength = xMessageBufferReceive(xMessageBuffer, pcBuffer, sizeof(pcBuffer), portMAX_DELAY);

        if (xReceivedLength > 0) {
            // Process the received message
            pcBuffer[xReceivedLength] = '\0'; // Null-terminate the string
            // Example: Print or use the message
        } else {
            // Handle error: Failed to receive the message or timeout occurred
        }

        // Simulate additional work
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
