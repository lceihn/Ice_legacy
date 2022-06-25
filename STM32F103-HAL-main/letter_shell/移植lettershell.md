## 移植 lettershell 到STM32(HAL库) <span id="p0004"></span>
- lettershell 版本V3.1.1
- 建议调试时使用
- 资源占用最小RAM:0.5kB, ROM:0.3kB
- [使用方法参考](https://github.com/NevermindZZT/letter-shell)

### 移植步骤
- 复制letter_shell文件夹到工程目录下
- GCC编译器则需要额外在STM32F103X_FLASH.ld中的.rodata :里添加
```c 
_shell_command_start = .;
KEEP (*(shellCommand))
_shell_command_end = .;
 ```
- CMakelist中添加./letter_shell/inc路径, 包含头文件
- CMakelist中添加./letter_shell/src, 编译目录下的.c源文件
- main.c中包含头文件 

```c 
#include "shell_port.h"
```
- 需要初始化直接调用 
```c 
userShellInit();
```

- 添加需要导出的命令和变量
- 最后在Xshell 中调试
