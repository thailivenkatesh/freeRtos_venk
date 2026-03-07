# FreeRTOS Real-Time Application Development – STM32 Nucleo

This repository demonstrates core FreeRTOS concepts implemented on the STM32 Nucleo development board using Embedded C.

## Hardware Platform
- STM32 Nucleo Development Board
- ARM Cortex-M4 Microcontroller

## RTOS Features Implemented

### Task Management
- Multiple task scheduling
- Task priorities
- Task delays and timing

### Inter-Task Communication
- Queues
- Queue Sets
- Message Buffers
- Stream Buffers

### Synchronization Mechanisms
- Binary Semaphores
- Counting Semaphores
- Mutex

### Event Handling
- Event Groups
- Task Notifications

### Interrupt Integration
- ISR to Task signaling
- Interrupt-based task synchronization

## File Structure

| File | Concept |
|-----|------|
| multipleTask.c | Multiple task scheduling |
| queue.c | Queue communication |
| binarySemaphore.c | Binary semaphore |
| countingSemaphore.c | Counting semaphore |
| mutex.c | Mutex synchronization |
| eventGroupWithInterrupt.c | Event group with ISR |
| messageBufferWithInterrupt.c | Message buffer communication |
| tasknotify.c | Task notifications |

## Purpose

The project demonstrates practical usage of FreeRTOS primitives for real-time embedded systems and helps understand task synchronization, scheduling, and inter-task communication.

## Technologies Used

- Embedded C
- FreeRTOS
- STM32 HAL
