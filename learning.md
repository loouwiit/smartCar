# 连接脚本 lds
用于控制 ld，内含各段的大小、位置属性

不指定时使用内置默认脚本

gcc -T ./lds

# 编译器脚本 opt

* makefile 里用@引入
* gcc @file 参数
* sh 里 cat

# config.c & .h
ti 生成的

使用 SYSCFG_DL_init 初始化所有

# device.lds.genlibs
do not edit -- TI

