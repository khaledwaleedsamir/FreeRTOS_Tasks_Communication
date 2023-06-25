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

const int LowerBoundValues[] = {50, 80, 110, 140, 170, 200};
const int UpperBoundValues[] = {150, 200, 250, 300, 350, 400};

int iterationCounter = -1; //incremented before first run so it will be 0

int UniformDistRandom(int rangeLow, int rangeHigh)
{
    int num = (rand() % (rangeHigh - rangeLow + 1)) +rangeLow;
    return num;
}

//Required Variables for tasks

int SentMessages1 = 0;
int SentMessages2 = 0;
int SentMessages3 = 0;
int BlockedMessages1 = 0;
int BlockedMessages2 = 0;
int BlockedMessages3 = 0;
int ReceivedMessages = 0;

//QUEUE_HANDLE

#define QUEUE_SIZE 3
QueueHandle_t MessageQueue;

//SEMAPHORES

SemaphoreHandle_t Sender1Semaphore;
SemaphoreHandle_t Sender2Semaphore;
SemaphoreHandle_t Sender3Semaphore;
SemaphoreHandle_t ReceiverSemaphore;

//TIMERS

TimerHandle_t Sender1Timer;
TimerHandle_t Sender2Timer;
TimerHandle_t Sender3Timer;
TimerHandle_t ReceiverTimer;

//Required Variables for timers

int Tsender1;
int Tsender2;
int Tsender3;
int TotalTsender1;
int TotalTsender2;
int TotalTsender3;
#define Treceiver pdMS_TO_TICKS(100)

//TASKS_FUNCTIONS

void Sender1Task(void* pvParameters){
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	while(1){
		    BaseType_t Status;
		    char Message[50];

			TickType_t CurrentTimeTicks = xTaskGetTickCount(); //get current time in system ticks

			snprintf(Message, sizeof(Message), "Time is: %lu", (unsigned long)CurrentTimeTicks); //Time is XYZ

			char * Msg = Message;

			BaseType_t CheckSemaphore = xSemaphoreTake(Sender1Semaphore,portMAX_DELAY); //check if semaphore is available

			if(CheckSemaphore == pdTRUE){

				Status = xQueueSend(MessageQueue, &Msg, 0); //send pointer of the message to queue

				if(Status == pdPASS){SentMessages1++;} //increment sent messages if message sent to queue

				else{BlockedMessages1++;} //increment blocked messages if message couldn't be delivered to queue

				//printf("Sender1 Sent Msgs: %d \n Sender 1 Blocked Msgs: %d \n",SentMessages,BlockedMessages);
        }
	}
}

/*the following two sender tasks are similar just created different tasks to differentiate between sender tasks
in terms of sent and blocked messages*/

void Sender2Task(void* pvParameters){
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	while(1){
		BaseType_t Status;
		char Message[50];
	    TickType_t CurrentTimeTicks = xTaskGetTickCount();
		snprintf(Message, sizeof(Message), "Time is: %lu", (unsigned long)CurrentTimeTicks);
		char * Msg = Message;
		BaseType_t CheckSemaphore = xSemaphoreTake(Sender2Semaphore,portMAX_DELAY);
		if(CheckSemaphore == pdTRUE){
			Status = xQueueSend(MessageQueue, &Msg, 0);
			if(Status == pdPASS){SentMessages2++;}
			else{BlockedMessages2++;}
		    //printf("Sender2 Sent Msgs: %d \n Sender 2 Blocked Msgs: %d \n",SentMessages2,BlockedMessages2);
	    }
	}
}
void Sender3Task(void* pvParameters){
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	while(1){
		BaseType_t Status;
		char Message[50];
		TickType_t CurrentTimeTicks = xTaskGetTickCount();
		snprintf(Message, sizeof(Message), "Time is: %lu", (unsigned long)CurrentTimeTicks);
		char * Msg = Message;
		BaseType_t CheckSemaphore = xSemaphoreTake(Sender3Semaphore,portMAX_DELAY);
	    if(CheckSemaphore == pdTRUE){
			Status = xQueueSend(MessageQueue, &Msg, 0);
			if(Status == pdPASS){SentMessages3++;}
		    else{BlockedMessages3++;}
		    //printf("Sender3 Sent Msgs: %d \n Sender 3 Blocked Msgs: %d \n",SentMessages3,BlockedMessages3);
	    }
	}
}
void ReceiverTask(void* pvParameters){
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	while(1){
		BaseType_t Status;
		char * ReceivedMsg; //buffer to recieve the message in

		BaseType_t CheckSemaphore = xSemaphoreTake(ReceiverSemaphore, portMAX_DELAY); //check semaphore

		if(CheckSemaphore == pdTRUE){

			Status = xQueueReceive(MessageQueue, &ReceivedMsg, 0); //receive the message from queue in the created buffer

			if (Status == pdPASS){

				ReceivedMessages++; //if message was recieved increment the received messages variable

				//puts(ReceivedMsg); //uncomment to print every received message

				//printf("Total received messages = %d \n",ReceivedMessages);
	  }
	}
  }
}
//Reset Functions

void PrintStats(){
	    printf("***************** Iteration number %d *****************\n",(iterationCounter+1));
	    printf("Total Sent Messages of Task 1: %d \n",SentMessages1);
		printf("Total Blocked Messages of Task 1: %d \n",BlockedMessages1);
		printf("Total Sent Messages of Task 2: %d \n",SentMessages2);
		printf("Total Blocked Messages of Task 2: %d \n",BlockedMessages2);
		printf("Total Sent Messages of Task 3: %d \n",SentMessages3);
		printf("Total Blocked Messages of Task 3: %d \n",BlockedMessages3);
		printf("Total Sent Messages: %d \n", (SentMessages1+SentMessages2+SentMessages3));
		printf("Total Received Messages: %d \n", ReceivedMessages);
		printf("Total Blocked Messages: %d \n",(BlockedMessages1+BlockedMessages2+BlockedMessages3));
		printf("Average Time of Sender1: %d \n",(TotalTsender1)/(SentMessages1+BlockedMessages1));
		printf("Average Time of Sender2: %d \n",(TotalTsender2)/(SentMessages2+BlockedMessages2));
		printf("Average Time of Sender3: %d \n",(TotalTsender3)/(SentMessages3+BlockedMessages3));
}
void ResetVariables(){
	SentMessages1 = 0;
	SentMessages2 = 0;
	SentMessages3 = 0;
	BlockedMessages1 = 0;
	BlockedMessages2 = 0;
	BlockedMessages3 = 0;
	ReceivedMessages = 0;
	TotalTsender1 = 0;
	TotalTsender2 = 0;
	TotalTsender3 = 0;
}
void Reset(){
	ResetVariables();
	xQueueReset(MessageQueue);
	iterationCounter++;
	if(iterationCounter>5){
		xTimerDelete(Sender1Timer,0);
		xTimerDelete(Sender2Timer,0);
		xTimerDelete(Sender3Timer,0);
		printf("Game Over \n");
		exit(0);
		vTaskEndScheduler();
	}
}

//Timers callback functions

static void Sender1TimerCallback(TimerHandle_t Sender1Timer){
	//generating another random value for Tsender
	Tsender1 = UniformDistRandom(LowerBoundValues[iterationCounter],UpperBoundValues[iterationCounter]);

	TotalTsender1+=Tsender1; //Total Tsender time to calculate the average

	xTimerChangePeriod(Sender1Timer,pdMS_TO_TICKS(Tsender1),0); //changing the timer's period to the new generated number

	//printf("Tsender1: %d \n",Tsender1);

	xSemaphoreGive(Sender1Semaphore); //giving the dedicated semaphore that the task is waiting on to execute
}

/* the other 2 senders timers function are similar to the first one*/

static void Sender2TimerCallback(TimerHandle_t Sender2Timer){
	Tsender2 = UniformDistRandom(LowerBoundValues[iterationCounter], UpperBoundValues[iterationCounter]);
	TotalTsender2+=Tsender2;
	xTimerChangePeriod(Sender2Timer,pdMS_TO_TICKS(Tsender2),0);
	//printf("Tsender2: %d \n",Tsender2);
	xSemaphoreGive(Sender2Semaphore);
}
static void Sender3TimerCallback(TimerHandle_t Sender3Timer){
	Tsender3 = UniformDistRandom(LowerBoundValues[iterationCounter],UpperBoundValues[iterationCounter]);
	TotalTsender3+=Tsender3;
	xTimerChangePeriod(Sender3Timer,pdMS_TO_TICKS(Tsender3),0);
	//printf("Tsender3: %d \n",Tsender3);
	xSemaphoreGive(Sender3Semaphore);
}
static void ReceiverTimerCallback(TimerHandle_t RecieverTimer){
	xSemaphoreGive(ReceiverSemaphore);
	if(ReceivedMessages >= 1000){
		PrintStats();
		Reset();
	}
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
	//semaphore creation
	Sender1Semaphore = xSemaphoreCreateBinary();
    Sender2Semaphore = xSemaphoreCreateBinary();
    Sender3Semaphore = xSemaphoreCreateBinary();
    ReceiverSemaphore = xSemaphoreCreateBinary();

    srand(xTaskGetTickCount()); //seed for rand function

    MessageQueue = xQueueCreate(QUEUE_SIZE,sizeof(char*)); //queue creation
	if(MessageQueue != NULL){
		 Reset();
		 xTaskCreate(Sender1Task, "Sender 1", 1000, (void*)1,1,NULL);
		 xTaskCreate(Sender2Task, "Sender 2", 1000, (void*)1,1,NULL);
	     xTaskCreate(Sender3Task, "Sender 3", 1000, (void*)1,2,NULL);
		 xTaskCreate(ReceiverTask, "Receiver", 1000, (void*)1,3,NULL);

		 //generating first random value for the 3 sender timers periods
		Tsender1 = UniformDistRandom(LowerBoundValues[iterationCounter],UpperBoundValues[iterationCounter]);
		Tsender2 = UniformDistRandom(LowerBoundValues[iterationCounter],UpperBoundValues[iterationCounter]);
		Tsender3 = UniformDistRandom(LowerBoundValues[iterationCounter],UpperBoundValues[iterationCounter]);
		TotalTsender1+=Tsender1;
		TotalTsender2+=Tsender2;
		TotalTsender3+=Tsender3;

		/* creating timers using the random values generated, note that the sender timers are created periodic
		 * but their periods will change after every call back function to a random value (check callback function)*/

		Sender1Timer = xTimerCreate("Sender1 Timer",pdMS_TO_TICKS(Tsender1),pdTRUE,(void*)1,Sender1TimerCallback);
		Sender2Timer = xTimerCreate("Sender2 Timer",pdMS_TO_TICKS(Tsender2),pdTRUE,(void*)2,Sender2TimerCallback);
		Sender3Timer = xTimerCreate("Sender3 Timer",pdMS_TO_TICKS(Tsender3),pdTRUE,(void*)3,Sender3TimerCallback);

		//creating receiver timer with fixed periodic time Treceiver

		ReceiverTimer = xTimerCreate("Receiver Timer",Treceiver,pdTRUE,(void*)4,ReceiverTimerCallback);

		/* starting timers and scheduler and wait for timers to give semaphores to tasks to start execution */

		xTimerStart(Sender1Timer,0);
		xSemaphoreTake(Sender1Semaphore,0);
		xTimerStart(Sender2Timer,0);
		xSemaphoreTake(Sender2Semaphore,0);
		xTimerStart(Sender3Timer,0);
		xSemaphoreTake(Sender3Semaphore,0);
		xTimerStart(ReceiverTimer,0);
		xSemaphoreTake(ReceiverSemaphore,0);

		vTaskStartScheduler();
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

