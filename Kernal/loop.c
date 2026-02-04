/**
 * @file      : loop.c
 * @brief     :
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Jan 27, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jan 27, 2026 | 1.0 | Drinkto | xxx |
 */

#include "loop.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

#include "tx_api.h"

#define DEMO_STACK_SIZE      1024
#define DEMO_BYTE_POOL_SIZE  9120
#define DEMO_BLOCK_POOL_SIZE 100
#define DEMO_QUEUE_SIZE      100

/* Define the ThreadX object control blocks...  */

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
TX_BYTE_POOL byte_pool_0;
TX_BLOCK_POOL block_pool_0;
UCHAR memory_area[DEMO_BYTE_POOL_SIZE];

/* 定义演示应用程序中所使用的计数器…… */
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

/* 定义线程原型 */
void thread_0_entry(ULONG thread_input);
void thread_1_entry(ULONG thread_input);
void thread_2_entry(ULONG thread_input);
void thread_3_and_4_entry(ULONG thread_input);
void thread_5_entry(ULONG thread_input);
void thread_6_and_7_entry(ULONG thread_input);
/**
 * @brief     : 主循环 用于替代main函数,防止CubeMX重新生成时覆盖
 *              c程序入口
 */
void loop(void)
{
        /* 内核开启前关闭HAL的时间基准 */
        HAL_SuspendTick();

        /* 进入ThreadX内核 */
        tx_kernel_enter();

        /* 正常不会运行到这里,运行到这里证明出错了 */
        for (;;) {
                // HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
                HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, GPIO_PIN_SET);

                HAL_GPIO_TogglePin(LED2_R_GPIO_Port, LED2_R_Pin);
                HAL_Delay(500);

                HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_TogglePin(LED1_G_GPIO_Port, LED1_G_Pin);
                HAL_Delay(500);

                // HAL_GPIO_WritePin(ENA1_GPIO_Port, ENA1_Pin, GPIO_PIN_RESET);
        }
}

/*
*******************************************************************************
*	函 数 名: tx_application_define
*	功能说明: ThreadX专用的任务创建，通信组件创建函数
*	形    参: first_unused_memory  未使用的地址空间
*	返 回 值: 无
*******************************************************************************
*/
void tx_application_define(void *first_unused_memory)
{
        CHAR *pointer = TX_NULL;

        /* 创建一个字节内存池，用于分配线程栈 */
        tx_byte_pool_create(&byte_pool_0,         /* 任务控制块池地址 */
                            "byte pool 0",        /* 任务控制块池名 */
                            memory_area,          /* 任务控制块池基地址 */
                            DEMO_BYTE_POOL_SIZE); /* 任务控制块池的字节数 */
        
        /* 为线程 0 分配栈空间 */
        tx_byte_allocate(&byte_pool_0,      /* 任务控制块池地址 */
                         (VOID **)&pointer, /* 堆栈基地址 */
                         DEMO_STACK_SIZE,   /* 栈空间大小 */
                         TX_NO_WAIT);       /* 无等待立即创建 */
        /* 将系统定义的相关内容放在这里，例如线程创建信息以及其他各类创建相关信息 */





        /* 创建主线程 */
        tx_thread_create(&thread_0,        /* 任务控制块地址 */
                         "thread 0",       /* 任务名 */
                         thread_0_entry,   /* 启动任务函数地址 */
                         0,                /* 传递给任务的参数 */
                         pointer,          /* 堆栈基地址 */
                         DEMO_STACK_SIZE,  /* 堆栈空间大小 */
                         1,                /* 任务优先级 */
                         1,                /* 任务抢占阈值 */
                         TX_NO_TIME_SLICE, /* 不开启时间片 */
                         TX_AUTO_START);   /* 创建后立即启动 */

        /* 为线程 1 分配栈空间 */
        tx_byte_allocate(&byte_pool_0,      /* 任务控制块池地址 */
                         (VOID **)&pointer, /* 堆栈基地址 */
                         DEMO_STACK_SIZE,   /* 栈空间大小 */
                         TX_NO_WAIT);       /* 无等待立即创建 */

        /* 创建线程 1 和 2 这些线程通过 ThreadX 消息队列传递信息 值得一提的是，这些线程还具有时间片的概念 */
        tx_thread_create(&thread_1,       /* 任务控制块地址 */
                         "thread 1",      /* 任务名 */
                         thread_1_entry,  /* 启动任务函数地址 */
                         1,               /* 传递给任务的参数 */
                         pointer,         /* 堆栈基地址 */
                         DEMO_STACK_SIZE, /* 堆栈空间大小 */
                         16,              /* 任务优先级 */
                         16,              /* 任务抢占阈值 */
                         4,               /* 时间片 */
                         TX_AUTO_START);  /* 创建后立即启动 */

        /* 为线程 2 分配栈空间 */
        tx_byte_allocate(&byte_pool_0,      /* 任务控制块池地址 */
                         (VOID **)&pointer, /* 堆栈基地址 */
                         DEMO_STACK_SIZE,   /* 栈空间大小 */
                         TX_NO_WAIT);       /* 无等待立即创建 */

        tx_thread_create(&thread_2,       /* 任务控制块地址 */
                         "thread 2",      /* 任务名 */
                         thread_2_entry,  /* 启动任务函数地址 */
                         2,               /* 传递给任务的参数 */
                         pointer,         /* 堆栈基地址 */
                         DEMO_STACK_SIZE, /* 堆栈空间大小 */
                         16,              /* 任务优先级 */
                         16,              /* 任务抢占阈值 */
                         4,               /* 时间片 */
                         TX_AUTO_START);  /* 创建后立即启动 */

        /* 为线程 3 分配栈空间 */
        tx_byte_allocate(&byte_pool_0,      /* 任务控制块池地址 */
                         (VOID **)&pointer, /* 堆栈基地址 */
                         DEMO_STACK_SIZE,   /* 栈空间大小 */
                         TX_NO_WAIT);       /* 无等待立即创建 */

        /* 创建线程 3 和 4 这些线程会争夺一个 ThreadX 计数信号量 这里有一个有趣的现象：这两个线程共享相同的指令区域 */
        tx_thread_create(&thread_3,            /* 任务控制块地址 */
                         "thread 3",           /* 任务名 */
                         thread_3_and_4_entry, /* 启动任务函数地址 */
                         3,                    /* 传递给任务的参数 */
                         pointer,              /* 堆栈基地址 */
                         DEMO_STACK_SIZE,      /* 堆栈空间大小 */
                         8,                    /* 任务优先级 */
                         8,                    /* 任务抢占阈值 */
                         TX_NO_TIME_SLICE,     /* 不开启时间片 */
                         TX_AUTO_START);       /* 创建后立即启动 */

        /* 为线程 4 分配栈空间 */
        tx_byte_allocate(&byte_pool_0,      /* 任务控制块池地址 */
                         (VOID **)&pointer, /* 堆栈基地址 */
                         DEMO_STACK_SIZE,   /* 栈空间大小 */
                         TX_NO_WAIT);       /* 无等待立即创建 */

        tx_thread_create(&thread_4,            /* 任务控制块地址 */
                         "thread 4",           /* 任务名 */
                         thread_3_and_4_entry, /* 启动任务函数地址 */
                         4,                    /* 传递给任务的参数 */
                         pointer,              /* 堆栈基地址 */
                         DEMO_STACK_SIZE,      /* 堆栈空间大小 */
                         8,                    /* 任务优先级 */
                         8,                    /* 任务抢占阈值 */
                         TX_NO_TIME_SLICE,     /* 不开启时间片 */
                         TX_AUTO_START);       /* 创建后立即启动 */

        /* 为线程 5 分配栈空间 */
        tx_byte_allocate(&byte_pool_0,      /* 任务控制块池地址 */
                         (VOID **)&pointer, /* 堆栈基地址 */
                         DEMO_STACK_SIZE,   /* 栈空间大小 */
                         TX_NO_WAIT);       /* 无等待立即创建 */

        /* 创建第 5 个线程 此线程只是等待一个事件标志，该标志将由线程 0 来设置 */
        tx_thread_create(&thread_5,        /* 任务控制块地址 */
                         "thread 5",       /* 任务名 */
                         thread_5_entry,   /* 启动任务函数地址 */
                         5,                /* 传递给任务的参数 */
                         pointer,          /* 堆栈基地址 */
                         DEMO_STACK_SIZE,  /* 堆栈空间大小 */
                         4,                /* 任务优先级 */
                         4,                /* 任务抢占阈值 */
                         TX_NO_TIME_SLICE, /* 不开启时间片 */
                         TX_AUTO_START);   /* 创建后立即启动 */

        /* 为线程 6 分配栈空间 */
        tx_byte_allocate(&byte_pool_0,      /* 任务控制块池地址 */
                         (VOID **)&pointer, /* 堆栈基地址 */
                         DEMO_STACK_SIZE,   /* 栈空间大小 */
                         TX_NO_WAIT);       /* 无等待立即创建 */

        /* 创建线程 6 和 7  这两个线程会争夺一个 ThreadX 互斥锁 */
        tx_thread_create(&thread_6,            /* 任务控制块地址 */
                         "thread 6",           /* 任务名 */
                         thread_6_and_7_entry, /* 启动任务函数地址 */
                         6,                    /* 传递给任务的参数 */
                         pointer,              /* 堆栈基地址 */
                         DEMO_STACK_SIZE,      /* 堆栈空间大小 */
                         8,                    /* 任务优先级 */
                         8,                    /* 任务抢占阈值 */
                         TX_NO_TIME_SLICE,     /* 不开启时间片 */
                         TX_AUTO_START);       /* 创建后立即启动 */

        /* 为线程 7 分配栈空间 */
        tx_byte_allocate(&byte_pool_0,      /* 任务控制块池地址 */
                         (VOID **)&pointer, /* 堆栈基地址 */
                         DEMO_STACK_SIZE,   /* 栈空间大小 */
                         TX_NO_WAIT);       /* 无等待立即创建 */

        tx_thread_create(&thread_7,            /* 任务控制块地址 */
                         "thread 7",           /* 任务名 */
                         thread_6_and_7_entry, /* 启动任务函数地址 */
                         7,                    /* 传递给任务的参数 */
                         pointer,              /* 堆栈基地址 */
                         DEMO_STACK_SIZE,      /* 堆栈空间大小 */
                         8,                    /* 任务优先级 */
                         8,                    /* 任务抢占阈值 */
                         TX_NO_TIME_SLICE,     /* 不开启时间片 */
                         TX_AUTO_START);       /* 创建后立即启动 */

        /* 分配消息队列 */
        tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);

        /* 创建由线程 1 和 2 共享的消息队列 */
        tx_queue_create(&queue_0,                         /* 消息队列地址 */
                        "queue 0",                        /* 消息队列名 */
                        TX_1_ULONG,                       /* 消息队列大小 */
                        pointer,                          /* 消息队列基地址 */
                        DEMO_QUEUE_SIZE * sizeof(ULONG)); /* 消息队列长度 */

        /* 创建供线程 3 和 4 使用的信号量 */
        tx_semaphore_create(&semaphore_0,  /* 信号量地址 */
                            "semaphore 0", /* 信号量名 */
                            1);            /* 信号量可用数量 */

        /* 创建供线程 1 和 5 使用的事件标志组 */
        tx_event_flags_create(&event_flags_0,   /* 事件标志组地址 */
                              "event flags 0"); /* 事件标志组名 */

        /* 创建供线程 6 和 7 使用的互斥锁，且不进行优先级继承 */
        tx_mutex_create(&mutex_0,       /* 互斥锁地址 */
                        "mutex 0",      /* 互斥锁名 */
                        TX_NO_INHERIT); /* 互斥量计数,不进行优先级继承 */

        /* 分配用于小型块池的内存 */
        tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_BLOCK_POOL_SIZE, TX_NO_WAIT);

        /* 创建一个块内存池，以便从其中分配消息缓冲区 */
        tx_block_pool_create(&block_pool_0, "block pool 0", sizeof(ULONG), pointer, DEMO_BLOCK_POOL_SIZE);

        /* 分配一块内存并释放该内存块 */
        tx_block_allocate(&block_pool_0, (VOID **)&pointer, TX_NO_WAIT);

        /* 将该块释放回资源池中*/
        tx_block_release(pointer);
}

/* 以下定义测试线程  */
/**
 * @brief     : 线程0 简单的while-forever-sleep循环
 * @param     : thread_input
 */
void thread_0_entry(ULONG thread_input)
{
        UINT status;

        /* 这个线程只是简单的处于while-forever-sleep循环,用于测试线程的基本功能 */
        while (1) {
                /* Increment the thread counter.  */
                thread_0_counter++;

                /* Sleep for 10 ticks.  */
                tx_thread_sleep(10);
                HAL_GPIO_TogglePin(LED2_R_GPIO_Port, LED2_R_Pin);
                tx_thread_sleep(1000);
                HAL_GPIO_TogglePin(LED1_G_GPIO_Port, LED1_G_Pin);

                /* Set event flag 0 to wakeup thread 5.  */
                status = tx_event_flags_set(&event_flags_0, 0x1, TX_OR);

                /* Check status.  */
                if (status != TX_SUCCESS)
                        break;
        }
}

/**
 * @brief     : 线程1 简单的向由线程 2 共享的队列发送消息
 * @param     : thread_input
 */
void thread_1_entry(ULONG thread_input)
{
        UINT status;

        /* 向由线程 2 共享的队列发送消息  */
        while (1) {
                /* Increment the thread counter.  */
                thread_1_counter++;

                /* 通过队列0发送消息  */
                status = tx_queue_send(&queue_0, &thread_1_messages_sent, TX_WAIT_FOREVER);

                /* Check completion status.  */
                if (status != TX_SUCCESS)
                        break;

                /* Increment the message sent.  */
                thread_1_messages_sent++;
        }
}

/**
 * @brief     : 线程2 获取由线程 1 放入队列中的消息
 * @param     : thread_input
 */
void thread_2_entry(ULONG thread_input)
{
        ULONG received_message;
        UINT status;

        /* 此线程负责获取由线程 1 放入队列中的消息 */
        while (1) {
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

/**
 * @brief     : 线程3 和 线程4 争夺 semaphore_0 的控制权
 * @param     : thread_input
 */
void thread_3_and_4_entry(ULONG thread_input)
{
        UINT status;

        /* 此函数由线程 3 和线程 4 执行  如下面的循环所示，这两个函数会争夺 semaphore_0 的控制权  */
        while (1) {
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
                tx_thread_sleep(2);

                /* Release the semaphore.  */
                status = tx_semaphore_put(&semaphore_0);

                /* Check status.  */
                if (status != TX_SUCCESS)
                        break;
        }
}

/**
 * @brief     : 线程5 等待事件标志0的发生
 * @param     : thread_input
 */
void thread_5_entry(ULONG thread_input)
{
        UINT status;
        ULONG actual_flags;

        /* 此线程会进入一个无限循环，持续等待某个事件的发生  */
        while (1) {
                /* Increment the thread counter.  */
                thread_5_counter++;

                /* Wait for event flag 0.  */
                status = tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);

                /* Check status.  */
                if ((status != TX_SUCCESS) || (actual_flags != 0x1))
                        break;
        }
}

/**
 * @brief     : 线程6 和 线程7 争夺 mutex_0 的所有权
 * @param     : thread_input
 */
void thread_6_and_7_entry(ULONG thread_input)
{
        UINT status;

        /* 此函数由线程 6 和线程 7 执行  如下面的循环所示，这两个函数会争夺 mutex_0 的所有权  */
        while (1) {
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
                tx_thread_sleep(2);

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