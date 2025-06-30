# hello config
供MSPM0G3507 sdk、TI sysconfig和code开发的空项目

# 配置环境
1. MSPM0G3507
2. arm-none-eabi-gcc
3. MSPM0 sdk
4. TI sysconfig
5. VS code
6. pyocd

# 使用

## 配置项目环境

1. 下载该项目
2. 在 `.vscode/c_cpp_properties.json` 中配置sdk的include目录
3. 在 `makefile` 中配置 `MSPM0_SDK_INSTALL_DIR`

## 修改项目名称
4. 修改文件夹名称为项目名称
5. 修改 `helloConfig.syscfg` 为项目名称
6. 在 `makefile`中 配置 `NAME` 为项目名称
7. 在 `.vscode/launch.json` 中配置 `executable` 路径，将文件名称改为项目名称

## TI sysconfig

8. 使用sysconfig修改配置，将所有生成的文件保存到项目目录

## 编写代码

9. 在 `src/` 下编写C/C++代码，make将自动编译相应.c/.cpp文件

如需添加额外的编译文件夹请修改 `makefile` 中的 `CPP_FILES` 和 `C_FILES`

## 调整编译器选项（可选）

10. 调整 `makefile` 中的 `CFLAGS`、`CPPFLAGS` 和 `LFLAGS`

# 可能的错误

### 找不到sysconfig和gcc
sdk下的`imports.mak` 中需要手动指定sysconfig和gcc的路径
