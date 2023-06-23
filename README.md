# HoloCubic-MicroPython-Utils

## 警告  
本项目含有以下要素：  
- 缺少注释  
- 不会写C  
- 不怎么会处理边界情况  
- “你完全不空行是吧”  
- 专有格式  

## 字体文件格式  
- 字体大小， 1字节  
- 压缩级别， 1字节  
- 中文字符映射表长度， 2字节  
- 中文字符映射表  
- ASCII字符（0x0000\~0x007F）字形  
- 半角标点字符（0x2000\~0x206F）字形  
- 全角标点字符（0x3000\~0x303F, 0xFF01\~0xFF5E）字形  
- 中文字符（常用字表）字形  

### 压缩级别  
可选0, 1, 2, 3，分别代表8bpp, 4bpp, 2bpp, 1bpp，默认1。  
每增加1，字形图片的体积减半，但是能表示的颜色数量会开方。  
对于字形来说，16色效果不错，所以默认1。  

#### 颜色压缩方式  
以8bpp->4bpp为例，简单的方法是直接每个数右移4位，解压时左移4位。这种方法看起来很理想：丢弃了4位数据，损失一定精度，获得压缩，但是有一个问题：它的实际效果不行。  
```
0xFF(255) -右移4位-> 0xF(15) -左移4位-> 0xF0(240)
```
我们发现一个很严重的问题：原色的范围是0\~255，但是还原的颜色的范围只有0\~240。如果这么压缩，我们就得和纯白说再见了。  
因此我们更换压缩方式：直接将0\~255按比例映射到0\~15，即：
```python
# python支持用中文做变量名，所以下面这一行其实是合法的
压缩值 = round(原值*15/255)
```

#### 颜色解压方式  
没啥好说的，就是压缩方式反过来。  
但由于解压是在开发板上完成的，而浮点运算代价很大，因此...  
我们打表！众所周知，打表出省一（滑稽  
实现详见natmod/myfont/_unsquash_dat.c  

#### 颜色存储方式  
存储的最小单位是一个字节，如果将颜色压缩后直接存储，文件大小并不会变小。因此，我们需要用一些手段来将多个压缩的颜色塞进一个字节里，并在需要时还原。  
实现详见font2bin/main.py - pack_byte()和natmod/myfont/_unsquash_dat.c  

### 中文字符映射表  
由于存储空间限制，不能将所有中文字符塞进字体文件，而使用的常用字表并不连续，于是就有了中文字符映射表。  
是一个从小到大排列的列表，满足：  
```python
中文字符映射表[中文字符在中文字符字形中的索引] = ord(中文字符)
```
因此：  
```python
bisect_left(中文字符映射表, ord(中文字符)) = 中文字符在中文字符字形中的索引
```
这种查找比字典映射快。  
注：micropython没有bisect模块，bisect_left的实现见natmod/bisect  
