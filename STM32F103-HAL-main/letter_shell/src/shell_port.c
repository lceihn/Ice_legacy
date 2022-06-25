/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */
#include "shell_port.h"

Shell shell;
uint8_t rx_data;
char shellBuffer[512] = {0};

/**
* @brief 串口接收1个字符进入 接收完成中断函数, 调用
* @param huart
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        shellHandler(&shell, rx_data);
    }
}

/**
 * @brief 用户shell写入
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际写入的数据长�?
 */
short userShellWrite(char *data, unsigned short len)
{
    HAL_UART_Transmit(&HUARTX, (uint8_t *)data, len, 0x1FF);
    return len;
}

/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    shell.write = userShellWrite;
    shellInit(&shell, shellBuffer, 512);
    HAL_UART_Receive_DMA(&HUARTX, &rx_data, 1);
}


