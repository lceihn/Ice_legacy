/**
 * @file shell_port.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#ifndef __SHELL_PORT_H__
#define __SHELL_PORT_H__

#include "shell.h"
#include "usart.h"

#define HUARTX huart1   // todo 使用哪个串口端口作为shell交互

extern Shell shell;

void userShellInit(void);

#endif
