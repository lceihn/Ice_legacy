#ifndef __DS18B20_H
#define	__DS18B20_H



#include "stm32f1xx_hal.h"



/************************** DS18B20 需要修改的地方, 端口设置********************************/
#define      DS18B20_DQ_SCK_ENABLE                     __HAL_RCC_GPIOB_CLK_ENABLE()

#define      DS18B20_DQ_GPIO_PORT                      GPIOB
#define      DS18B20_DQ_GPIO_PIN                       GPIO_PIN_11



/************************** DS18B20 不需要修改********************************/
#define      DS18B20_DQ_0	                            HAL_GPIO_WritePin(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, GPIO_PIN_RESET)
#define      DS18B20_DQ_1	                            HAL_GPIO_WritePin(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, GPIO_PIN_SET)

#define      DS18B20_DQ_IN()	                        HAL_GPIO_ReadPin(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)



/************************** DS18B20 函数实现 ********************************/
uint8_t                  DS18B20_Init                      ( void );
void                     DS18B20_ReadId                    (uint8_t * ds18b20_id );
float                    DS18B20_GetTemp_SkipRom           ( void );
float                    DS18B20_GetTemp_MatchRom          (uint8_t * ds18b20_id );
void                     delay_us                          (unsigned int Delay);


#endif /* __DS18B20_H */





