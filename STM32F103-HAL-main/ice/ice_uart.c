/**
 * @file ice_uart.c
 * @brief stm32 uart standred communication
 * @author lceihn 
 * @version 1.0
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021  806277022@qq.com
 * 
 * @par log:2021-12-10: version 1.0
 *          todo 当DMA接收一帧数据超过256时可能会存在一些问题
 */
#include "ice_uart.h"

/* Include */


/* static function */
static uint8_t isCheck(IceUart *ice);
static uint8_t checkSum(uint8_t *buf);
static void clear(IceUart *ice);
static void sendError(IceUart *ice);

/**
* @brief 串口的任务处理方法的具体实现
*/
void ice_uart_task(IceUart *ice)
{
    if (ice->rx_flag) //received data
    {
        if (isCheck(ice))
        {
            ice->error = Ack_OK;

            switch (ice->rx_buf[CMD])
            {
                case 0x01:
                {

                    break;
                }
                default:
                    ice->error = CMD_Err;
                    break;
            }
        }

        if (ice->error)
            sendError(ice);
        clear(ice);
        ice->rx_flag = 0;    
        HAL_UART_Receive_DMA(ice->huart, ice->rx_buf, ice_dma_size);
    }
}

/**
 * @brief 中断函数, 需要添加到stm32f1xx_it.c中对应的UARTX_Handler处理函数中
 */
void ice_uart_it_callback(IceUart *ice)
{
    if (__HAL_UART_GET_FLAG(ice->huart, UART_FLAG_IDLE) != RESET)  //USARTX_IRQHandler中断处理函数中添加, 接收到1帧数据触发空闲中断
    {
        __HAL_UART_CLEAR_IDLEFLAG(ice->huart);    //clear flag

        if (ice->rx_flag == 0) //1:busy(processing uart data)
        {
            ice->rx_len = ice_dma_size - __HAL_DMA_GET_COUNTER(ice->huart->hdmarx);    //获取接收到的数据长度
            HAL_UART_DMAStop(ice->huart);    //停止DMA接收
            ice->rx_flag = 1;             //标记接收到1帧数据
        }
    }
}

/**
 * @brief stm32 uart init, start uart dma receive
 */
void ice_uart_init(IceUart *ice, UART_HandleTypeDef *huart)
{
    ice->huart = huart;
    ice->rx_flag = 0;
    __HAL_UART_ENABLE_IT(ice->huart, UART_IT_IDLE); //enable idle
    HAL_UART_Receive_DMA(ice->huart, ice->rx_buf, ice_dma_size);   //start uart dma receive
}

/**
 * @brief  return check ok ?
 * @param  ice              
 * @return 
 */
static uint8_t isCheck(IceUart *ice)
{
    uint8_t ok_flag = 1;
    ice->error = Ack_OK;

    if (ice->rx_len != ice_uart_size)
    {
        ice->error = Packtet_Size_Err;
        ok_flag = 0;
    }
    else if (ice->rx_buf[HEAD] != ice_head
        || ice->rx_buf[END] != ice_end)
    {
        ice->error = Head_Or_End_Err;
        ok_flag = 0;
    }
    else
    {
        uint8_t check = checkSum(ice->rx_buf);

        if (check != ice->rx_buf[CHECK])
        {
            ice->error = Check_Err;
            ok_flag = 0;
        }
    }

    return ok_flag;
}

/**
* @brief use sum check
* @param ice
* @return
*/
static uint8_t checkSum(uint8_t *buf)
{
    uint8_t sum = 0;
    for (uint8_t i = CMD; i < CHECK; ++i)
        sum += buf[i];
    return sum;
}


/**
 * @brief 将接收缓冲区内容清0
 * @param ice_buf
 */
static void clear(IceUart *ice)
{
    for (uint8_t i = 0; i < ice->rx_len; ++i)
    {
        ice->rx_buf[i] = 0;
    }
}

/**
* @brief communication false will return message to host, CMD = 0xff, ACK = ice->error
* @param ice
*/
static void sendError(IceUart *ice)
{
    uint8_t buf[ice_uart_size] = {0};

    buf[HEAD] = ice_head;
    buf[END] = ice_end;
    buf[CMD] = 0xff;
    buf[ACK] = ice->error;
    buf[CHECK] = checkSum(buf);

    HAL_UART_Transmit(ice->huart, buf, ice_uart_size, 10);
}




#if ice_debug   

#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

PUTCHAR_PROTOTYPE
{
    //注意下面第一个参数是&huartx，因为cubemx配置了串口x自动生成的
    while (__HAL_UART_GET_FLAG(&ice_debug_uart, UART_FLAG_TC) == 0);
    ice_debug_uart.Instance->DR = (uint8_t) ch;
    return ch;
}
#endif

#endif