NAME = smartCar

STARTUP_FILE = $(MSPM0_SDK_INSTALL_DIR)/source/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0g350x_gcc.c

CPP_FILES = $(wildcard src/*.cpp)
C_FILES = $(wildcard src/*.c) syscalls.c ti_msp_dl_config.c $(STARTUP_FILE)

MSPM0_SDK_INSTALL_DIR ?= $(abspath /opt/ti/mspm0_sdk_2_11_00_07)

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

SYSCONFIG_GUI_TOOL = $(dir $(SYSCONFIG_TOOL))sysconfig_gui$(suffix $(SYSCONFIG_TOOL))
SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler gcc --product $(MSPM0_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_GUI_CMD_STUB = $(SYSCONFIG_GUI_TOOL) --compiler gcc --product $(MSPM0_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ./$(NAME).syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))
SYSCFG_OPT_FILES = $(filter %.opt,$(SYSCFG_FILES))

OBJECTS = $(patsubst %.cpp,%.obj,$(notdir $(CPP_FILES))) $(patsubst %.c,%.obj,$(notdir $(C_FILES))) $(patsubst %.c,%.obj,$(notdir $(SYSCFG_C_FILES)))

CFLAGS += -I. \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    -O2 \
    @device.opt \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source" \
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
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

CPPFLAGS += -I. \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    -O0 \
    @device.opt \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source" \
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
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

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
    "-L$(GCC_ARMCOMPILER)/arm-none-eabi/lib/thumb/v6-m/nofp" \
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
