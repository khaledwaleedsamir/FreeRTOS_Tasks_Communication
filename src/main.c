/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

//----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diag/trace.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#define CCM_RAM __attribute__((section(".ccmram")))

//Uniform Distribution Random Number Generation function
int LowerBoundValues[] = {50, 80, 110, 140, 170, 200};
int UpperBoundValues[] = {150, 200, 250, 300, 350, 400};

int iterationCounter = 0;

int UniformDistRandom(int rangeLow, int rangeHigh)
{
    srand(xTaskGetTickCount());
    int num = (rand() % (rangeHigh - rangeLow + 1)) +rangeLow;
    return num;
}

//QUEUE_HANDLE
#define QUEUE_SIZE 3
QueueHandle_t MessageQueue;

//TASKS_FUNCTIONS
void Sender1Task(void* pvParameters){
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	int SentMessages = 0;
	int BlockedMessages = 0;
	BaseType_t Status;
	while(1){
		    char Message[50];
			TickType_t CurrentTimeTicks = xTaskGetTickCount();
			snprintf(Message, sizeof(Message), "Time is: %lu", (unsigned long)CurrentTimeTicks);
			char * Msg = Message;
			Status = xQueueSend(MessageQueue, &Msg, 0);
			if(Status == pdPASS){SentMessages++;}
			else{BlockedMessages++;}
            printf("Sender1 Sent Msgs: %d \n Sender 1 Blocked Msgs: %d \n",SentMessages,BlockedMessages);
			//const TickType_t ticksdelay = pdMS_TO_TICKS(1000);
			//vTaskDelay(ticksdelay);
	        }
}
void Sender2Task(void* pvParameters){
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	int SentMessages = 0;
	int BlockedMessages = 0;
	BaseType_t Status;
	while(1){
		    char Message[50];
			TickType_t CurrentTimeTicks = xTaskGetTickCount();
			snprintf(Message, sizeof(Message), "Time is: %lu", (unsigned long)CurrentTimeTicks);
			char * Msg = Message;
			Status = xQueueSend(MessageQueue, &Msg, 0);
			if(Status == pdPASS){SentMessages++;}
			else{BlockedMessages++;}
            printf("Sender2 Sent Msgs: %d \n Sender 2 Blocked Msgs: %d \n",SentMessages,BlockedMessages);
			const TickType_t ticksdelay = pdMS_TO_TICKS(1000);
			vTaskDelay(ticksdelay);
	        }
}
void Sender3Task(void* pvParameters){
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	int SentMessages = 0;
	int BlockedMessages = 0;
	BaseType_t Status;
	while(1){
		    char Message[50];
			TickType_t CurrentTimeTicks = xTaskGetTickCount();
			snprintf(Message, sizeof(Message), "Time is: %lu", (unsigned long)CurrentTimeTicks);
			char * Msg = Message;
			Status = xQueueSend(MessageQueue, &Msg, 0);
			if(Status == pdPASS){SentMessages++;}
			else{BlockedMessages++;}
            printf("Sender3 Sent Msgs: %d \n Sender  Blocked Msgs: %d \n",SentMessages,BlockedMessages);
			const TickType_t ticksdelay = pdMS_TO_TICKS(1000);
			vTaskDelay(ticksdelay);
	        }
}
void ReceiverTask(void* pvParameters){
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	int ReceivedMessages = 0;
	while(1){
		BaseType_t Status;
		char * ReceivedMsg;
		const TickType_t ticksToWait = pdMS_TO_TICKS(1000);
		Status = xQueueReceive(MessageQueue, &ReceivedMsg, ticksToWait);
		if (Status == pdPASS){
			ReceivedMessages++;
			puts(ReceivedMsg);
			printf("Total received messages = %d \n",ReceivedMessages);
		}
		else{
			printf("Couldn't receive from Queue! \n");
		}
	}
}

//SEMAPHORES
SemaphoreHandle_t Sender1Semaphore;
SemaphoreHandle_t Sender2Semaphore;
SemaphoreHandle_t Sender3Semaphore;
SemaphoreHandle_t ReceiverSemaphore;

//TIMERS
TimerHandle_t Sender1Timer;
TimerHandle_t Sender2Timer;
TimerHandle_t Sender3Timer;
TimerHandle_t RecieverTimer;
int Tsender1;
int Tsender2;
int Tsender3;
#define Treceiver pdMS_TO_TICKS(100)


static void Sender1TimerCallback(TimerHandle_t SenderTimer){
	Tsender1 = UniformDistRandom(LowerBoundValues[iterationCounter],LowerBoundValues[iterationCounter]);
	xSemaphoreGive(Sender1Semaphore);
}
static void Sender2TimerCallback(TimerHandle_t SenderTimer){
	Tsender2 = UniformDistRandom(LowerBoundValues[iterationCounter],LowerBoundValues[iterationCounter]);
	xSemaphoreGive(Sender2Semaphore);
}
static void Sender3TimerCallback(TimerHandle_t SenderTimer){
	Tsender3 = UniformDistRandom(LowerBoundValues[iterationCounter],LowerBoundValues[iterationCounter]);
	xSemaphoreGive(Sender3Semaphore);
}
static void RecieverTimerCallback(TimerHandle_t RecieverTimer){
	xSemaphoreGive(ReceiverSemaphore);
}

/*-----------------------------------------------------------*/
// ----------------------------------------------------------------------------
//
// Semihosting STM32F4 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace-impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).


// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{

	MessageQueue = xQueueCreate(QUEUE_SIZE,sizeof(char*));
	if(MessageQueue != NULL){

	}
	else{
		printf("Queue Could not be created \n");
	}
	return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------


void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

void vApplicationTickHook(void) {
}

StaticTask_t xIdleTaskTCB CCM_RAM;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE] CCM_RAM;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
  state will be stored. */
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

  /* Pass out the array that will be used as the Idle task's stack. */
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;

  /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
  Note that, as the array is necessarily of type StackType_t,
  configMINIMAL_STACK_SIZE is specified in words, not bytes. */
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB CCM_RAM;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH] CCM_RAM;

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

