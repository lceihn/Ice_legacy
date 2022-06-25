## 移植 RT-Thread Nano (通过Stm32cubeMx + CLion)

- 裸机资源占用 **RAM: 2.5KB** + **ROM: 4KB**
- Nano 资源占用 **RAM: 1KB** + **ROM: 10KB**
- 如果需要丰富的组件、驱动以及软件包等功能，则建议使用 RT-Thread 完整版
- 移植版本为: **RT-Thread Nano 3.1.5**



### 将代码添加到app_rt_thread.c (新建线程文件)

```c
#include "app_rt_thread.h"

int MX_RT_Thread_Init(void)
{
    //初始化线程
    rt_err_t rst = RT_EOK;
    rst += rt_thread_init(&led_thread,                          /* 线程 */
                         (const char *)"ledshine",       		/* 线程名字 */
                         led_thread_entry,                      /* 线程入口函数 */
                         RT_NULL,                               /* 线程入口函数参数 */
                         &led_thread_stack[0],       			/* 线程栈入口地址 */
                         sizeof(led_thread_stack),              /* 线程栈大小 */
                         RT_THREAD_PRIORITY_MAX - 2,    		/* 线程的优先级 */
                         20);                             		/* 线程时间片 */


    if(rst == RT_EOK)
    {  /* 启动线程，开启调度 */
        rt_thread_startup(&led_thread);
    }

    return rst;
}

/**
 * @brief LED灯闪烁任务线程的实现
 * @param parameter 未使用
 */
static void led_thread_entry(void *parameter)
{
    while(1)
    {
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
        rt_thread_mdelay(100);
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
        rt_thread_mdelay(100);
    }
}
```

### 将代码添加到app_rt_thread.h

```c
#ifndef _APP_RT_THREAD_H_
#define _APP_RT_THREAD_H_

#include "rtthread.h"
#include "main.h"

//所有线程初始化函数
int MX_RT_Thread_Init(void);

/* 定义线程控制块 */
/* 线程一 */
static struct rt_thread led_thread;
static char led_thread_stack[256];
static void led_thread_entry(void *parameter);

#endif
```

### 如果要使用finsh组件，在STM32F103XXX_FLASH.ld文件内的.text 后添加

```c
/* section information for finsh shell */
. = ALIGN(4);
__fsymtab_start = .;
KEEP(*(FSymTab))
__fsymtab_end = .;
. = ALIGN(4);
__vsymtab_start = .;
KEEP(*(VSymTab))
__vsymtab_end = .;
. = ALIGN(4);

/* section information for initial. */
. = ALIGN(4);
__rt_init_start = .;
KEEP(*(SORT(.rti_fn*)))
__rt_init_end = .;
. = ALIGN(4);
```

