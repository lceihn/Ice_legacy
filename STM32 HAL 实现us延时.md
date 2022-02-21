## STM32 HAL 实现us延时

```c
/*
Systick功能实现us延时，参数SYSCLK为系统时钟
*/
#define STM32_SYSCLK (200) //todo 修改主频200MHz
void HAL_Delay_us(uint32_t nus)
{
    static uint32_t fac_us = STM32_SYSCLK;
    uint32_t ticks;
    uint32_t told,tnow,tcnt=0;
    uint32_t reload=SysTick->LOAD;
    ticks=nus*fac_us;
    told=SysTick->VAL;
    while(1)
    {
        tnow=SysTick->VAL;
        if(tnow!=told)
        {
            if(tnow<told)tcnt+=told-tnow;
            else tcnt+=reload-tnow+told;
            told=tnow;
            if(tcnt>=ticks)break;
        }
    }
}
```