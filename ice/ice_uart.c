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
static uint8_t isChecked(IceUart *ice);
static uint8_t checkSum(uint8_t *buf);
static void clear(IceUart *ice);
static void sendData(IceUart *ice);
static void sendError(IceUart *ice);

/**
* @brief uart received data handle
*/
void ice_uart_task(IceUart *ice)
{
    if (ice->rx_flag) //received data
    {
        ice->error = Ack_OK;
        uint8_t ok = isChecked(ice); //校验数据

        if (ok) //校验通过
        {
            switch (ice->rx_buf[CMD])
            {
                case 0x20:
                    break;
                default:
                    ice->error = CMD_Err; //不存在对应的指令
                    break;
            }
        }

        if (ice->error) //error in transmit
            sendError(ice);

        clear(ice);
        HAL_UART_Receive_DMA(ice->huart, ice->rx_buf, ice_dma_size); //开启新的dma接收
#if ice_usb
        USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS); //开启新的usb接收
        USBD_CDC_ReceivePacket(&hUsbDeviceFS);
#endif
    }
}

/**
 * @brief 中断回调函数, 需要添加到stm32f1xx_it.c中对应的UARTX_Handler处理函数中
 */
void ice_uart_it_callback(IceUart *ice)
{
    if (__HAL_UART_GET_FLAG(ice->huart, UART_FLAG_IDLE) != RESET)  //接收到1帧数据触发空闲中断
    {
        __HAL_UART_CLEAR_IDLEFLAG(ice->huart);    //clear idle flag

        if (ice->rx_flag == 0) //1:busy(processing uart data)
        {
            ice->rx_len = ice_dma_size - __HAL_DMA_GET_COUNTER(ice->huart->hdmarx); //获取接收到的数据长度
            HAL_UART_DMAStop(ice->huart); //停止DMA接收, 处理完数据之后再打开
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
static uint8_t isChecked(IceUart *ice)
{
    uint8_t ok = 1;
    ice->error = Ack_OK;

    if (ice->rx_len != ice_uart_size) //数据大小是否符合协议标准
    {
        ice->error = Packtet_Size_Err;
        ok = 0;
    }
    else if (ice->rx_buf[HEAD] != ice_head
        || ice->rx_buf[END] != ice_end) //数据帧头和帧尾是否符合协议标准
    {
        ice->error = Head_Or_End_Err;
        ok = 0;
    }
    else
    {
        uint8_t check = checkSum(ice->rx_buf);

        if (check != ice->rx_buf[CHECK]) //检验码错误
        {
            ice->error = Check_Err;
            ok = 0;
        }
    }

    return ok;
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
 * @brief 将接收缓冲区内容清0, 清空接收标志位
 * @param ice_buf
 */
static void clear(IceUart *ice)
{
    for (uint8_t i = 0; i < ice->rx_len; ++i)
        ice->rx_buf[i] = 0;

    ice->rx_flag = 0;
}

/**
* @brief send data
* @param ice
*/
static void sendData(IceUart *ice)
{
    ice->tx_buf[HEAD] = ice_head;
    ice->tx_buf[END] = ice_end;
    ice->tx_buf[CHECK] = checkSum(ice->tx_buf);

    HAL_UART_Transmit(ice->huart, ice->tx_buf, ice_uart_size, 50);
#if ice_usb
    CDC_Transmit_FS(ice->tx_buf, ice_uart_size);
#endif
}

/**
* @brief communication false will return message to host, CMD = 0xff, ACK = ice->error
* @param ice
*/
static void sendError(IceUart *ice)
{
    ice->tx_buf[CMD] = 0xff;
    ice->tx_buf[ACK] = ice->error;

    sendData(ice);
}

#if ice_debug
#define ice_debug_uart  huart1  //uart x ? to use printf

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

#if ice_usb

/**
* @brief usb port reset
*/
void ice_usb_reset()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);
    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_Delay(10);
}

/**
* @brief usbd_cdc_if.c中添加的接收回调函数
* @param ice
* @param Buf
* @param Len
*/
void ice_usb_rxcallback(IceUart *ice, uint8_t *Buf, uint32_t *Len)
{
    ice->rx_flag = 1;
    ice->rx_len = *Len;
    for (int i = 0; i < *Len; ++i)
    {
        ice->rx_buf[i] = Buf[i];
    }
    ice_uart_task(ice);
}

#endif