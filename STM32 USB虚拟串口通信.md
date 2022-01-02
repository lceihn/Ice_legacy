## STM32 USB虚拟串口通信

- STM32CubeMx配置USB_DEVICE为 CDC类(Virtual Port Com), 改一下USB接收中断的优先级, 其他默认      额外占用资源 4kB + 12kB</font>

- `MX_USB_DEVICE_Init();`初始化之前添加`USB_Reset()`

- **usbd_cdc_if.c 中对应都收发函数中添加自己对应处理函数**

  - `CDC_Receive_FS()` 接收数据回调函数

  - `CDC_Transmit_FS()` 用来发送数据

- **注意:** <font color=red>默认配置的通信数据为64个字节为1包, 上位机尝试发送超过64个字节会存在问题</font>
  - 包大小的宏定义在**usbd_cdc.h** (<font color=red>不建议修改</font>)

- [相关资料](https://blog.csdn.net/Naisu_kun/article/details/118192032)

- **ice.uart.h** 配置 ice_usb为1, 开启usb功能

- **usbd_cdc_if.c **中添加**ice_uart.h**, 修改`CDC_Receive_FS()`

  - ```c
    #if ice_usb
        ice_usb_rxcallback(&ice_uart1, Buf, Len);
    //    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
    //    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    #else
        USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
        USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    #endif
    ```



```c
void USB_Reset()
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
```

