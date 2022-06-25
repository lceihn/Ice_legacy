/**
 * @file ice_uart.h
 * @brief stm32 uart standred communication
 * @author lceihn 
 * @version 1.0
 * @date 2021-12-10
 * 
 * @copyright Copyright (c) 2021  806277022@qq.com
 * 
 * @par log: 2021-12-10: version 1.0
 *           todo 当DMA接收一帧数据超过256时可能会存在一些问题
 */

#ifndef _ICE_UART_H_
#define _ICE_UART_H_

/* Include */
#include "usart.h"

/* config uart */
#define ice_dma_size        256     //dma rx size
#define ice_uart_size       16      //packet size
#define ice_head            0x11    //packet head
#define ice_end             0x21    //packet end

#define ice_debug           1       //printf via uart, 0:disable 1:enable
#if ice_debug
#define ice_debug_uart      huart1  //uart x ? to use printf   
#endif

/* packet bytes definition */
#define HEAD                0		//head
#define CMD                 1	    //command
#define ADDR                2       //address
#define DB1                 3	    //data byte 1
#define DB2                 4	    //data byte 2
#define DB3                 5	    //data byte 3
#define DB4                 6	    //data byte 4
#define DB5                 7	    //data byte 5
#define DB6                 8	    //data byte 6
#define DB7                 9	    //data byte 7
#define DB8                 10	    //data byte 8
#define DB9                 11	    //data byte 9
#define NONE                12	    //not used
#define ACK                 13	    //ack
#define CHECK               14	    //check
#define END                 15		//end

/* ice uart structure */
typedef struct
{
    UART_HandleTypeDef *huart;      //pointer to stm32 huart x
    uint8_t rx_buf[ice_dma_size];   //ice uart receive data buffer(maxlength:256)
    uint8_t rx_flag;                //ice uart rx flag, 0:wait for receive, 1:received packet data
    uint8_t rx_len;                 //ice uart actually receive packet length
    uint8_t error;                  //ice error
}IceUart;


/* error definition */
typedef enum
{
    Ack_OK = 0,                     //communication normal
    Packtet_Size_Err,               //packet size error
    Head_Or_End_Err,                //head or end error
    Check_Err,                      //check error
    CMD_Err,                        //command error
}Ack_Flag_Type;

/* global variable */
extern IceUart ice_uart1;

/* ice uart function */
void ice_uart_init(IceUart *ice, UART_HandleTypeDef *huart);    //ice uart init
void ice_uart_it_callback(IceUart *ice);                        //ice uart interrupt callback, add to stm32f1xx_it.c void USARTx_IRQHandler(void)
void ice_uart_task(IceUart *ice);		                        //process uart receive task

#endif //_ICE_UART_H_
