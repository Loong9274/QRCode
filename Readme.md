## 二维码生成器
图一乐的用于生成二维码的C库
当前状态：仅支持生成"HELLO WORLD"

### 目标
- 正确 完全符合二维码标准
- 快速 速度接近或优于现有库
- 体积小 占用存储与内存空间尽可能小

### 期望特性
- 支持二维码版本1~40，并可裁剪以减少空间占用
- 支持由外部进行内存管理，不申请释放内存

### 项目进度
| 小目标                | 情况  |
| ------------------ | --- |
| 设计好api             |     |
| 能按Tutorial生成可扫的二维码 |     |
| 支持版本1~40           |     |
| 支持裁剪               |     |

### 体验
```shell
# windows
cd build
cmake -G"MinGW Makefiles" ..
make
.\test\testByMinifb.exe

# linux
mkdir build
cmake ..
make
./test/testByMinifb
    #   you may meet this error (i did)
    #   error xkbcommon/xkbcommon.h: No such file or directory
    #   sudo apt-get install libxkbcommon-x11-dev
```

### 参考资料
1. [Thonky's QR Code Tutorial](https://www.thonky.com/qr-code-tutorial/)

### 测试使用到的开源库
1. [minifb](https://github.com/emoon/minifb)

