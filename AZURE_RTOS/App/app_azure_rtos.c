/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_azure_rtos.c
 * @author  MCD Application Team
 * @brief   app_azure_rtos application implementation file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_azure_rtos.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEMO_STACK_SIZE 1024
//#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_BLOCK_POOL_SIZE 100
#define DEMO_QUEUE_SIZE 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
TX_THREAD thread_0;
TX_THREAD thread_1;
TX_THREAD thread_2;
TX_THREAD thread_3;
TX_THREAD thread_4;
TX_THREAD thread_5;
TX_THREAD thread_6;
TX_THREAD thread_7;
TX_QUEUE queue_0;
TX_SEMAPHORE semaphore_0;
TX_MUTEX mutex_0;
TX_EVENT_FLAGS_GROUP event_flags_0;
// TX_BYTE_POOL            tx_app_byte_pool;
TX_BLOCK_POOL block_pool_0;
// UCHAR                   memory_area[DEMO_BYTE_POOL_SIZE];

ULONG thread_0_counter;
ULONG thread_1_counter;
ULONG thread_1_messages_sent;
ULONG thread_2_counter;
ULONG thread_2_messages_received;
ULONG thread_3_counter;
ULONG thread_4_counter;
ULONG thread_5_counter;
ULONG thread_6_counter;
ULONG thread_7_counter;
/* USER CODE END PV */

#if (USE_STATIC_ALLOCATION == 1)

/* USER CODE BEGIN TX_Pool_Buffer */
/* USER CODE END TX_Pool_Buffer */
#if defined(__ICCARM__)
#pragma data_alignment = 4
#endif
__ALIGN_BEGIN static UCHAR tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE] __ALIGN_END;
static TX_BYTE_POOL tx_app_byte_pool;

/* USER CODE BEGIN NX_Pool_Buffer */
/* USER CODE END NX_Pool_Buffer */
#if defined(__ICCARM__)
#pragma data_alignment = 4
#endif
__ALIGN_BEGIN static UCHAR nx_byte_pool_buffer[NX_APP_MEM_POOL_SIZE] __ALIGN_END;
static TX_BYTE_POOL nx_app_byte_pool;

#endif

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void thread_0_entry(ULONG thread_input)
{

    UINT status;

    /* This thread simply sits in while-forever-sleep loop.  */
    while (1)
    {
    	printf("%ld\r\n",thread_input);
        /* Increment the thread counter.  */
        thread_0_counter++;

        /* Sleep for 10 ticks.  */
        tx_thread_sleep(100);

        /* Set event flag 0 to wakeup thread 5.  */
        status = tx_event_flags_set(&event_flags_0, 0x1, TX_OR);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}

void thread_1_entry(ULONG thread_input)
{

    UINT status;

    /* This thread simply sends messages to a queue shared by thread 2.  */
    while (1)
    {
//    	printf("%ld\r\n",thread_input);
        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Send message to queue 0.  */
        status = tx_queue_send(&queue_0, &thread_1_messages_sent, TX_WAIT_FOREVER);

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;

        /* Increment the message sent.  */
        thread_1_messages_sent++;
    }
}

void thread_2_entry(ULONG thread_input)
{

    ULONG received_message;
    UINT status;

    /* This thread retrieves messages placed on the queue by thread 1.  */
    while (1)
    {
//    	printf("%ld\r\n",thread_input);
        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Retrieve a message from the queue.  */
        status = tx_queue_receive(&queue_0, &received_message, TX_WAIT_FOREVER);

        /* Check completion status and make sure the message is what we
           expected.  */
        if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received))
            break;

        /* Otherwise, all is okay.  Increment the received message count.  */
        thread_2_messages_received++;
    }
}

void thread_3_and_4_entry(ULONG thread_input)
{

    UINT status;

    /* This function is executed from thread 3 and thread 4.  As the loop
       below shows, these function compete for ownership of semaphore_0.  */
    while (1)
    {
        printf("%ld\r\n",thread_input);
        /* Increment the thread counter.  */
        if (thread_input == 3)
            thread_3_counter++;
        else
            thread_4_counter++;

        /* Get the semaphore with suspension.  */
        status = tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the semaphore.  */
        tx_thread_sleep(20);

        /* Release the semaphore.  */
        status = tx_semaphore_put(&semaphore_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}

void thread_5_entry(ULONG thread_input)
{

    UINT status;
    ULONG actual_flags;

    /* This thread simply waits for an event in a forever loop.  */
    while (1)
    {
    	printf("%ld\r\n",thread_input);
        /* Increment the thread counter.  */
        thread_5_counter++;

        /* Wait for event flag 0.  */
        status = tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR,
                                    &actual_flags, TX_WAIT_FOREVER);

        /* Check status.  */
        if ((status != TX_SUCCESS) || (actual_flags != 0x1))
            break;
    }
}

void thread_6_and_7_entry(ULONG thread_input)
{

    UINT status;

    /* This function is executed from thread 6 and thread 7.  As the loop
       below shows, these function compete for ownership of mutex_0.  */
    while (1)
    {
    	printf("%ld\r\n",thread_input);
        /* Increment the thread counter.  */
        if (thread_input == 6)
            thread_6_counter++;
        else
            thread_7_counter++;

        /* Get the mutex with suspension.  */
        status = tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Get the mutex again with suspension.  This shows
           that an owning thread may retrieve the mutex it
           owns multiple times.  */
        status = tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the mutex.  */
        tx_thread_sleep(20);

        /* Release the mutex.  */
        status = tx_mutex_put(&mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Release the mutex again.  This will actually
           release ownership since it was obtained twice.  */
        status = tx_mutex_put(&mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}
/* USER CODE END PFP */

/**
 * @brief  Define the initial system.
 * @param  first_unused_memory : Pointer to the first unused memory
 * @retval None
 */
VOID tx_application_define(VOID *first_unused_memory)
{
    /* USER CODE BEGIN  tx_application_define_1*/

    /* USER CODE END  tx_application_define_1 */
#if (USE_STATIC_ALLOCATION == 1)
    UINT status = TX_SUCCESS;
    VOID *memory_ptr = TX_NULL;

    if (tx_byte_pool_create(&tx_app_byte_pool, "Tx App memory pool", tx_byte_pool_buffer, TX_APP_MEM_POOL_SIZE) != TX_SUCCESS)
    {
        /* USER CODE BEGIN TX_Byte_Pool_Error */

        /* USER CODE END TX_Byte_Pool_Error */
    }
    else
    {
        /* USER CODE BEGIN TX_Byte_Pool_Success */
        /* Allocate the stack for thread 0.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_STACK_SIZE, TX_NO_WAIT);

        /* Create the main thread.  */
        tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,
                         memory_ptr, DEMO_STACK_SIZE,
                         1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

        /* Allocate the stack for thread 1.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_STACK_SIZE, TX_NO_WAIT);

        /* Create threads 1 and 2. These threads pass information through a ThreadX
           message queue.  It is also interesting to note that these threads have a time
           slice.  */
        tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,
                         memory_ptr, DEMO_STACK_SIZE,
                         16, 16, 4, TX_AUTO_START);

        /* Allocate the stack for thread 2.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_STACK_SIZE, TX_NO_WAIT);

        tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,
                         memory_ptr, DEMO_STACK_SIZE,
                         16, 16, 4, TX_AUTO_START);

        /* Allocate the stack for thread 3.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_STACK_SIZE, TX_NO_WAIT);

        /* Create threads 3 and 4.  These threads compete for a ThreadX counting semaphore.
           An interesting thing here is that both threads share the same instruction area.  */
        tx_thread_create(&thread_3, "thread 3", thread_3_and_4_entry, 3,
                         memory_ptr, DEMO_STACK_SIZE,
                         8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

        /* Allocate the stack for thread 4.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_STACK_SIZE, TX_NO_WAIT);

        tx_thread_create(&thread_4, "thread 4", thread_3_and_4_entry, 4,
                         memory_ptr, DEMO_STACK_SIZE,
                         8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

        /* Allocate the stack for thread 5.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_STACK_SIZE, TX_NO_WAIT);

        /* Create thread 5.  This thread simply pends on an event flag which will be set
           by thread_0.  */
        tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,
                         memory_ptr, DEMO_STACK_SIZE,
                         4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

        /* Allocate the stack for thread 6.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_STACK_SIZE, TX_NO_WAIT);

        /* Create threads 6 and 7.  These threads compete for a ThreadX mutex.  */
        tx_thread_create(&thread_6, "thread 6", thread_6_and_7_entry, 6,
                         memory_ptr, DEMO_STACK_SIZE,
                         8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

        /* Allocate the stack for thread 7.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_STACK_SIZE, TX_NO_WAIT);

        tx_thread_create(&thread_7, "thread 7", thread_6_and_7_entry, 7,
                         memory_ptr, DEMO_STACK_SIZE,
                         8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

        /* Allocate the message queue.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);

        /* Create the message queue shared by threads 1 and 2.  */
        tx_queue_create(&queue_0, "queue 0", TX_1_ULONG, memory_ptr, DEMO_QUEUE_SIZE * sizeof(ULONG));

        /* Create the semaphore used by threads 3 and 4.  */
        tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

        /* Create the event flags group used by threads 1 and 5.  */
        tx_event_flags_create(&event_flags_0, "event flags 0");

        /* Create the mutex used by thread 6 and 7 without priority inheritance.  */
        tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);

        /* Allocate the memory for a small block pool.  */
        tx_byte_allocate(&tx_app_byte_pool, (VOID **)&memory_ptr, DEMO_BLOCK_POOL_SIZE, TX_NO_WAIT);

        /* Create a block memory pool to allocate a message buffer from.  */
        tx_block_pool_create(&block_pool_0, "block pool 0", sizeof(ULONG), memory_ptr, DEMO_BLOCK_POOL_SIZE);

        /* Allocate a block and release the block memory.  */
        tx_block_allocate(&block_pool_0, (VOID **)&memory_ptr, TX_NO_WAIT);

        /* Release the block back to the pool.  */
        tx_block_release(memory_ptr);
        /* USER CODE END TX_Byte_Pool_Success */
		memory_ptr = (VOID *)&tx_app_byte_pool;
		status = App_ThreadX_Init(memory_ptr);
		if (status != TX_SUCCESS)
		{
		  /* USER CODE BEGIN  App_ThreadX_Init_Error */
		  while(1)
		  {
		  }
		  /* USER CODE END  App_ThreadX_Init_Error */
		}
        /* USER CODE BEGIN  App_ThreadX_Init_Success */

        /* USER CODE END  App_ThreadX_Init_Success */
    }

    if (tx_byte_pool_create(&nx_app_byte_pool, "Nx App memory pool", nx_byte_pool_buffer, NX_APP_MEM_POOL_SIZE) != TX_SUCCESS)
    {
        /* USER CODE BEGIN NX_Byte_Pool_Error */

        /* USER CODE END NX_Byte_Pool_Error */
    }
    else
    {
        /* USER CODE BEGIN TX_Byte_Pool_Success */

        /* USER CODE END TX_Byte_Pool_Success */

        memory_ptr = (VOID *)&nx_app_byte_pool;
        status = MX_NetXDuo_Init(memory_ptr);
        if (status != NX_SUCCESS)
        {
            /* USER CODE BEGIN  MX_NetXDuo_Init_Error */
            while (1)
            {
            }
            /* USER CODE END  MX_NetXDuo_Init_Error */
        }
        /* USER CODE BEGIN  MX_NetXDuo_Init_Success */

        /* USER CODE END MX_NetXDuo_Init_Success */
    }
#else
    /*
     * Using dynamic memory allocation requires to apply some changes to the linker file.
     * ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
     * using the "first_unused_memory" argument.
     * This require changes in the linker files to expose this memory location.
     * For EWARM add the following section into the .icf file:
         place in RAM_region    { last section FREE_MEM };
     * For MDK-ARM
         - either define the RW_IRAM1 region in the ".sct" file
         - or modify the line below in "tx_low_level_initilize.s to match the memory region being used
            LDR r1, =|Image$$RW_IRAM1$$ZI$$Limit|

     * For STM32CubeIDE add the following section into the .ld file:
         ._threadx_heap :
           {
              . = ALIGN(8);
              __RAM_segment_used_end__ = .;
              . = . + 64K;
              . = ALIGN(8);
            } >RAM_D1 AT> RAM_D1
        * The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
        * In the example above the ThreadX heap size is set to 64KBytes.
        * The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
        * Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
        * Read more in STM32CubeIDE User Guide, chapter: "Linker script".

     * The "tx_initialize_low_level.s" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.
     */

    /* USER CODE BEGIN DYNAMIC_MEM_ALLOC */
    (void)first_unused_memory;
    /* USER CODE END DYNAMIC_MEM_ALLOC */
#endif
}
