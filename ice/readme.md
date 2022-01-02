## ice_uart

#### 步骤:

- ***step1***: STM32CubeMx添加串口, 开启DMA接收(**circle**), 开启UART和DMA的中断
- ***step2***: 复制**ice**文件夹到Src路径下, CMakeList添加(**ice_uart.h**)路径
- ***step3***: **main.c **中创建一个**IceUart**的全局变量, 在**ice_uart.h**中extern声明
- ***step4***: **stm32f1xx_it.c**中**`USART1_IRQHandler()`**中添加**`ice_uart_it_callback()`**, 添加**(ice_uart.h**)
- ***step5***: **main.c**添加(**ice_uart.h**), 初始化**`ice_uart_init()`**,循环中添加**`ice_uart_task()`**
- ***step6***: **ice_uart.h **中根据需要开启对应的宏, 配置对应的功能


#### BUG：

- 当一次性接收数据大于<font color=red>**256**</font>字节时, 接收数据大小<font color=blue>**ice_uart_len**</font>可能出现错误 

