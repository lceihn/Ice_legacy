## 移植easyflash到STM32(HAL库) <span id="p0003"></span>

- easyflash版本V4.1.99
- 只使用了easyflash的ENV功能
- 资源占用RAM:0.6kB, ROM:8kB
- [API参考](https://github.com/armink/EasyFlash/blob/master/docs/zh/api.md)



### 移植步骤

- 复制easyflash文件夹到工程目录下
- CMakelist中添加./easyflash/inc路径, 包含头文件
- CMakelist中添加./easyflash/src, port, 编译目录下的.c源文件
- ./easyflash/port/ef_port.c中添加默认的ENV
- ./easyflash/inc/ef_cfg.h中修改ENV大小, 备份区起始地址等相关配置
- 需要初始化直接调用 ```easyflash_init();           //初始化easyflash ```
- 最后直接调用需要的API即可

