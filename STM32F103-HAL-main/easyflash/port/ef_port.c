/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <easyflash.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* todo 配置默认的环境变量 */
/* default environment variables set for user */
//static uint32_t boot_count = 0;
//static time_t boot_time[10] = {0, 1, 2, 3};
static uint8_t ch_status[4] = {0, 0, 0, 0};                            //DAC通道的状态, 0 ~ 3 对应 A ~D, 数值参考AD5685R.h--->Line53
static uint16_t ch_value[4] = {16383, 16383, 16383, 16383};            //DAC通道的初始化电压值, 0 ~ 3 对应 A ~D

static const ef_env default_env_set[] = {
//        {   key  , value, value_len }，
    {"user", "lceihn", 0},                      //用户名
    {"password", "lc19991121", 0},              //密码
    {"device_num", "1", 0},                     //产品序列号
    {"version", "V1.0", 0},                     //固件版本
    {"ch_status", &ch_status, sizeof(ch_status)},
    {"ch_value", &ch_value, sizeof(ch_value)},
//        {"username", "armink", 0},   //类型为字符串的环境变量可以设定值的长度为 0 ，此时会在初始化时会自动检测其长度
//        {"password", "123456", 0},
//        {"boot_count", &boot_count, sizeof(boot_count)},  //整形
//        {"boot_time", &boot_time, sizeof(boot_time)},  //数组类型，其他类型使用方式类似
};

//static char log_buf[128];

/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size)
{
    EfErrCode result = EF_NO_ERR;

    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

    return result;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size)
{
    EfErrCode result = EF_NO_ERR;

    /* You can add your code under here. */
    uint8_t *buf_8 = (uint8_t *) buf;
    size_t i;

    /*copy from flash to ram */
    for (i = 0; i < size; i++, addr++, buf_8++)
    {
        *buf_8 = *(uint8_t *) addr;
    }

    return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size)
{
    EfErrCode result = EF_NO_ERR;

    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

    /* You can add your code under here. */
    uint32_t flash_status;
    size_t erase_pages, i;

    /* calculate pages */
    erase_pages = size / FLASH_PAGE_SIZE;
    if (size % FLASH_PAGE_SIZE != 0)
    {
        erase_pages++;
    }

    /* start erase */
    HAL_FLASH_Unlock();

    //定义一个擦除结构体的变量, 填入擦除相关的信息
    FLASH_EraseInitTypeDef Temp;

    Temp.TypeErase = FLASH_TYPEERASE_PAGES; //擦除的是页
    Temp.Banks = FLASH_BANK_1;  //页属于那一块, 貌似STM32F103XX都只有一个块
    //Temp.PageAddress = addr;  //当前地址所在的页
    Temp.NbPages = 1;   //擦除页的数量


    for (i = 0; i < erase_pages; i++)
    {
        Temp.PageAddress = addr + (FLASH_PAGE_SIZE * i);
        HAL_FLASHEx_Erase(&Temp, &flash_status);
        if (flash_status != 0xFFFFFFFF)
        {
            result = EF_ERASE_ERR;
            break;
        }
    }

    HAL_FLASH_Lock();

    return result;
}

/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size)
{
    EfErrCode result = EF_NO_ERR;

    /* You can add your code under here. */
    size_t i;
    uint32_t read_data;

    HAL_FLASH_Unlock();

    for (i = 0; i < size; i += 4, buf++, addr += 4)
    {
        /* write data */
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *buf);
        read_data = *(uint32_t *) addr;
        /* check data */
        if (read_data != *buf)
        {
            result = EF_WRITE_ERR;
            break;
        }
    }
    HAL_FLASH_Lock();

    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void)
{

    /* You can add your code under here. */
    __disable_irq();

}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void)
{

    /* You can add your code under here. */
    __enable_irq();

}


/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...)
{

#ifdef PRINT_DEBUG

    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
   
    /* You can add your code under here. */
    ef_print("[Flash](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsprintf(log_buf, format, args);
    ef_print("%s", log_buf);
    printf("\r");

    va_end(args);

#endif

}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
//    ef_print("[Flash]");
//    /* must use vprintf to print */
//    vsprintf(log_buf, format, args);
//    ef_print("%s", log_buf);
//    printf("\r");

    va_end(args);
}

/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
//    /* must use vprintf to print */
//    vsprintf(log_buf, format, args);
//    printf("%s", log_buf);

    va_end(args);
}
