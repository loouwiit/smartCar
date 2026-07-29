NAME = smartCar

STARTUP_FILE = $(MSPM0_SDK_INSTALL_DIR)/source/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0g350x_gcc.c

CPP_FILES = $(wildcard src/*.cpp)
FREERTOS_DIR = $(MSPM0_SDK_INSTALL_DIR)/kernel/freertos
FREERTOS_SRC = \
    $(FREERTOS_DIR)/Source/tasks.c \
    $(FREERTOS_DIR)/Source/queue.c \
    $(FREERTOS_DIR)/Source/list.c \
    $(FREERTOS_DIR)/Source/timers.c \
    $(FREERTOS_DIR)/Source/event_groups.c \
    $(FREERTOS_DIR)/Source/stream_buffer.c \
    $(FREERTOS_DIR)/Source/croutine.c \
    $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM0/port.c \
    $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM0/portasm.c \
    $(FREERTOS_DIR)/Source/portable/MemMang/heap_4.c

MSPM0_SDK_INSTALL_DIR ?= $(abspath /opt/ti/mspm0_sdk_2_11_00_07)

CC = arm-none-eabi-gcc
LNK = arm-none-eabi-g++
ARM_NONE_EABI ?= /usr/lib/arm-none-eabi

SYSCONFIG_TOOL ?= /opt/ti/sysconfig_1.28.0/sysconfig_cli.sh

SYSCONFIG_GUI_TOOL = $(dir $(SYSCONFIG_TOOL))sysconfig_gui$(suffix $(SYSCONFIG_TOOL))
SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler gcc --product $(MSPM0_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_GUI_CMD_STUB = $(SYSCONFIG_GUI_TOOL) --compiler gcc --product $(MSPM0_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ./$(NAME).syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))
SYSCFG_OPT_FILES = $(filter %.opt,$(SYSCFG_FILES))

# C_FILES: 排除 SysConfig 已生成的文件避免重复编译
# (ifeq 必须在 SYSCFG_C_FILES 定义之后才能正确判断)
ifeq ($(SYSCFG_C_FILES),)
C_FILES = $(wildcard src/*.c) syscalls.c ti_msp_dl_config.c $(STARTUP_FILE) $(FREERTOS_SRC)
else
C_FILES = $(wildcard src/*.c) syscalls.c $(FREERTOS_SRC)
endif

OBJECTS = $(patsubst %.cpp,%.obj,$(notdir $(CPP_FILES))) $(patsubst %.c,%.obj,$(notdir $(C_FILES))) $(patsubst %.c,%.obj,$(notdir $(SYSCFG_C_FILES)))

CFLAGS += -I. \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    -O2 \
    @device.opt \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source" \
    "-I$(FREERTOS_DIR)/Source/include" \
    "-I$(FREERTOS_DIR)/Source/portable/GCC/ARM_CM0" \
    -mcpu=cortex-m0plus \
    -march=armv6-m \
    -mthumb \
    -std=c99 \
    -mfloat-abi=soft \
    -ffunction-sections \
    -fdata-sections \
    -g \
    -gstrict-dwarf \
    -Wall \
    "-I$(ARM_NONE_EABI)/include/nano" \
    "-I$(ARM_NONE_EABI)/include"

CPPFLAGS += -I. \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    -O0 \
    @device.opt \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source" \
    "-I$(FREERTOS_DIR)/Source/include" \
    "-I$(FREERTOS_DIR)/Source/portable/GCC/ARM_CM0" \
    -mcpu=cortex-m0plus \
    -march=armv6-m \
    -mthumb \
    -std=c++11 \
    -mfloat-abi=soft \
    -ffunction-sections \
    -fdata-sections \
    -g \
    -gstrict-dwarf \
    -Wall \
    "-I$(ARM_NONE_EABI)/include/nano" \
    "-I$(ARM_NONE_EABI)/include"

LFLAGS += "-L$(MSPM0_SDK_INSTALL_DIR)/source/ti/driverlib/lib/gcc/m0p/mspm0g1x0x_g3x0x" \
    -nostartfiles \
    -Tdevice.lds.genlibs \
    -l:driverlib.a \
    -Wl,-T,device_linker.lds \
    -Wl,--no-warn-rwx-segments \
    "-Wl,-Map,build/$(NAME).map" \
    "-L$(MSPM0_SDK_INSTALL_DIR)/source" \
    -L. \
    -march=armv6-m \
    -mthumb \
    -static \
    -Wl,--gc-sections \
    "-L$(ARM_NONE_EABI)/lib/thumb/v6-m/nofp" \
    -lgcc \
    -lc \
    -lm \
    --specs=nano.specs \
    --specs=nosys.specs

all: $(NAME).elf

.INTERMEDIATE: syscfg
$(SYSCFG_FILES): syscfg
	@ echo generation complete

syscfg: ./$(NAME).syscfg
	@ echo Generating configuration files...
	@ $(SYSCFG_CMD_STUB) --output $(@D) $<

# Helpful hint that the user needs to use a standalone SysConfig installation
$(SYSCONFIG_GUI_TOOL):
	$(error $(dir $(SYSCONFIG_TOOL)) does not contain the GUI framework \
        necessary to launch the SysConfig GUI.  Please set SYSCONFIG_TOOL \
        (in your SDK's imports.mak) to a standalone SysConfig installation \
        rather than one inside CCS)

syscfg-gui: ./$(NAME).syscfg $(SYSCONFIG_GUI_TOOL)
	@ echo Opening SysConfig GUI
	@ $(SYSCFG_GUI_CMD_STUB) $<

define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	@ $(CC) $(CFLAGS) $$< -c -o build/$$@
	@ echo Finish building $$@
endef

$(foreach c_file,$(SYSCFG_C_FILES),$(eval $(call C_RULE,$(c_file))))

define CPP_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	@ $(CC) $(CPPFLAGS) $$< -c -o build/$$@
	@ echo Finish building $$@
endef

$(foreach cpp_file,$(CPP_FILES),$(eval $(call CPP_RULE,$(cpp_file))))

$(foreach c_file,$(C_FILES),$(eval $(call C_RULE,$(c_file))))

$(NAME).elf: $(OBJECTS)
	@ echo Linking $@
	@ $(LNK)  $(addprefix build/,$(OBJECTS))  $(LFLAGS) -o build/$(NAME).elf
	@ echo Finish linking $@
