TARGET     := mkrwifi1010
SKETCH     := src

# Board specific flags, TODO include it from bootloader definition
BOARD_FLAGS := -DUSB_VID=0x2341 -DUSB_PID=0x8054 -DF_CPU=48000000

# Directory Configuration
OBJDIR      := obj

# Arduino Core library
CORE         := ArduinoCore-samd/cores/arduino/ ArduinoCore-samd/cores/arduino/USB/
CORE_DIR     := $(addprefix lib/, $(CORE))
CORE_INC     := $(addprefix -I,$(CORE_DIR))
CORE_CC_SRC  := $(foreach dir,$(CORE_DIR), $(wildcard $(dir)*.c))
CORE_CXX_SRC := $(foreach dir,$(CORE_DIR), $(wildcard $(dir)*.cpp))

CORE_API         := ArduinoCore-API/
CORE_API_DIR     := $(addprefix lib/, $(CORE_API))
CORE_API_INC     := $(addprefix -I,$(CORE_API_DIR))
CORE_API_CC_SRC  := $(wildcard $(CORE_API_DIR)api/*.c)
CORE_API_CXX_SRC := $(wildcard $(CORE_API_DIR)api/*.cpp)

CORE_LIBRAIRIES         := SPI
CORE_LIBRAIRIES         += Wire
CORE_LIBRAIRIES_DIR     := $(addprefix lib/ArduinoCore-samd/libraries/, $(CORE_LIBRAIRIES))
CORE_LIBRAIRIES_INC     := $(foreach dir,$(CORE_LIBRAIRIES_DIR), $(addprefix -I,$(dir)))
CORE_LIBRAIRIES_CC_SRC  := $(foreach dir,$(CORE_LIBRAIRIES_DIR), $(wildcard $(dir)/*.c))
CORE_LIBRAIRIES_CXX_SRC := $(foreach dir,$(CORE_LIBRAIRIES_DIR), $(wildcard $(dir)/*.cpp))

LIBRARIES  := FlashStorage_SAMD
LIBRARIES  += SD
LIBRARIES  += RTCZero
LIBRARIES  += WiFiNINA

LIBRARIES_DIRS    += $(addsuffix /src/,$(addprefix lib/,$(LIBRARIES)))
LIBRARIES_INC     := $(addprefix -I,$(LIBRARIES_DIRS))
LIBRARIES_CC_SRC  := $(foreach dir,$(LIBRARIES_DIRS), $(wildcard $(dir)*.c))
LIBRARIES_CXX_SRC := $(foreach dir,$(LIBRARIES_DIRS), $(wildcard $(dir)*.cpp))
# Include 1 depth subfolders, TODO improve subfolder search
LIBRARIES_CC_SRC  += $(foreach dir,$(LIBRARIES_DIRS), $(wildcard $(dir)*/*.c))
LIBRARIES_CXX_SRC += $(foreach dir,$(LIBRARIES_DIRS), $(wildcard $(dir)*/*.cpp))

# FreeRTOS source
FREERTOS_DIR     := lib/FreeRTOS-Kernel
LIBRARIES_CC_SRC += $(foreach dir,$(FREERTOS_DIR), $(wildcard $(dir)/*.c))
LIBRARIES_CC_SRC += $(FREERTOS_DIR)/portable/GCC/ARM_CM0/port.c
LIBRARIES_CC_SRC += $(FREERTOS_DIR)/portable/MemMang/heap_3.c
LIBRARIES_INC    += -I$(FREERTOS_DIR)/portable/GCC/ARM_CM0
LIBRARIES_INC    += -I$(FREERTOS_DIR)/include
# FreeRTOS port
FREERTOS_PORT_DIR := lib/FreeRTOS_port
LIBRARIES_INC     += -I$(FREERTOS_PORT_DIR)
LIBRARIES_CC_SRC  += $(foreach dir,$(FREERTOS_PORT_DIR), $(wildcard $(dir)/*.c))
LIBRARIES_CXX_SRC += $(foreach dir,$(FREERTOS_PORT_DIR), $(wildcard $(dir)/*.cpp))
LIBRARIES_CC_SRC  += $(foreach dir,$(FREERTOS_PORT_DIR), $(wildcard $(dir)/*.c))

TARGET_CC_SRC  := $(wildcard $(SKETCH)/*.c)
TARGET_CXX_SRC := $(wildcard $(SKETCH)/*.cpp)
TARGET_CC_SRC  += $(wildcard $(SKETCH)/font/*.c)
TARGET_CC_SRC  += $(wildcard $(SKETCH)/drv/screen/*.c)
TARGET_CC_SRC  += $(wildcard $(SKETCH)/drv/timer/*.c)
TARGET_CC_SRC  += $(wildcard $(SKETCH)/ui/*.c)
TARGET_CXX_SRC += $(wildcard $(SKETCH)/ui/*.cpp)
TARGET_CC_SRC  += $(wildcard $(SKETCH)/fsm/*.c)
TARGET_CC_SRC  += $(wildcard $(SKETCH)/music/*.c)
TARGET_CXX_SRC += $(wildcard $(SKETCH)/music/*.cpp)
TARGET_CXX_SRC += $(wildcard $(SKETCH)/flash_storage/*.cpp)
TARGET_CXX_SRC += $(wildcard $(SKETCH)/wifi/*.cpp)
TARGET_CXX_SRC += $(wildcard $(SKETCH)/rtc/*.cpp)
TARGET_CXX_SRC += $(wildcard $(SKETCH)/button/*.cpp)

VARIANT_DIR := lib/ArduinoCore-samd/variants/mkrwifi1010
VARIANT_SRC := $(VARIANT_DIR)/variant.cpp
CMSIS_DIR   := lib/CMSIS_5/CMSIS/Core
SAM_DIR     := lib/ArduinoModule-CMSIS-Atmel/CMSIS-Atmel/CMSIS/Device/ATMEL

# List all .o files
CORE_OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(CORE_CC_SRC)) \
             $(patsubst %.cpp,$(OBJDIR)/%.o,$(CORE_CXX_SRC)) \
             $(patsubst %.cpp,$(OBJDIR)/%.o,$(CORE_API_CC_SRC)) \
             $(patsubst %.cpp,$(OBJDIR)/%.o,$(CORE_API_CXX_SRC)) \
             $(patsubst %.c,$(OBJDIR)/%.o,$(CORE_LIBRAIRIES_CC_SRC)) \
             $(patsubst %.cpp,$(OBJDIR)/%.o,$(CORE_LIBRAIRIES_CXX_SRC))

LIBRARIES_OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(LIBRARIES_CC_SRC)) \
                  $(patsubst %.cpp,$(OBJDIR)/%.o,$(LIBRARIES_CXX_SRC))

TARGET_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(TARGET_CXX_SRC)) \
              $(patsubst %.c,$(OBJDIR)/%.o,$(TARGET_CC_SRC))

VARIANT_OBJ := $(patsubst %.cpp,$(OBJDIR)/%.o,$(VARIANT_SRC))

SRCS := $(CORE_OBJS) $(LIBRARIES_OBJS) $(TARGET_OBJ) $(VARIANT_OBJ)

COMPORT     ?= /dev/ttyACM0
BOSSAC      ?= bossac
BOSSAC_FLAGS := --erase --write --verify --reset --usb-port --port=$(COMPORT)
OS          := $(shell uname -s)
ifeq ($(OS),Linux)
RESET_SCRIPT := scripts/reset-arduino-linux.sh -q $(COMPORT)
else
RESET_SCRIPT := scripts/ard-reset-arduino --zero $(COMPORT)
endif

# my bossa-git AUR package sets the version to the Arch pkgver, which is 1.8.rXX.gYYYYYYY since
# there hasn't been a v1.9 tag yet, so figure out the version by checking for the availability
# of the --arduino-erase option
BOSSA_19 := $(shell $(BOSSAC) --help 2>/dev/null | grep -q -e '--arduino-erase' && echo y || echo n)
ifeq ($(BOSSA_19),y)
# we have auto-erase available
BOSSAC_FLAGS := $(filter-out --erase,$(BOSSAC_FLAGS))
# BOSSA v1.8 hard-coded the flash starting address as 0x2000, so the command-line offset
# must be zero (the default) or else the program would get written to 0x4000.
# BOSSA v1.9 doesn't do that, so we must set the offset to 0x2000 or else the bootloader
# will get overwritten, bricking the board.
BOSSAC_FLAGS += --offset=0x2000
endif

# Tools Configuration
TOOLCHAIN_BIN ?=
export CC      = $(TOOLCHAIN_BIN)arm-none-eabi-gcc
export CXX     = $(TOOLCHAIN_BIN)arm-none-eabi-g++
export AS      = $(TOOLCHAIN_BIN)arm-none-eabi-gcc -x assembler-with-cpp
export CCLD    = $(TOOLCHAIN_BIN)arm-none-eabi-gcc
export CXXLD   = $(TOOLCHAIN_BIN)arm-none-eabi-g++
export AR      = $(TOOLCHAIN_BIN)arm-none-eabi-gcc-ar
export OBJCOPY = $(TOOLCHAIN_BIN)arm-none-eabi-objcopy
export OBJDUMP = $(TOOLCHAIN_BIN)arm-none-eabi-objdump
export SIZE    = $(TOOLCHAIN_BIN)arm-none-eabi-size
export GDB     = $(TOOLCHAIN_BIN)arm-none-eabi-gdb

# Sketches can set PRINTF_FLOAT=1 to build with this option (adds 12kB)
PRINTF_FLOAT_FLAG = -Wl,--require-defined=_printf_float

LCPPFLAGS  := -D__SAMD21G18A__ -DUSBCON $(SOURCE_VERSION_FLAG)
LCPPFLAGS  += -I$(SKETCH) $(CORE_INC) $(CORE_API_INC) $(CORE_LIBRAIRIES_INC) $(LIBRARIES_INC) -I$(CMSIS_DIR)/Include -I$(SAM_DIR) -I$(VARIANT_DIR)
LCPPFLAGS  += -MMD -MP

# common flags
CPUFLAGS   := -mcpu=cortex-m0plus -mthumb -ggdb3 -Os

# used in CFLAGS/CXXFLAGS/ASFLAGS, but not LDFLAGS
CCXXFLAGS  := $(BOARD_FLAGS) $(CPUFLAGS) -Wall -Wextra -Werror -Wno-unused-parameter -Wno-switch -Wno-ignored-qualifiers
CCXXFLAGS  += -fno-exceptions -ffunction-sections -fdata-sections -Wno-expansion-to-defined

# Too many warning in Arduino source code, silent them
CXXFLAGS_EXTRA_ARDUINO := -Wno-class-memaccess -Wno-address-of-packed-member -Wno-format-overflow -Wno-restrict -Wno-maybe-uninitialized 
CXXFLAGS_EXTRA_ARDUINO += -Wno-sized-deallocation -Wno-unused-variable -Wno-volatile
CFLAGS_EXTRA_ARDUINO += -Wno-enum-int-mismatch

LCFLAGS    := $(CCXXFLAGS) -std=gnu18

LCXXFLAGS  := $(CCXXFLAGS) -std=gnu++20 -fno-rtti -fno-threadsafe-statics

LASFLAGS   := $(CCXXFLAGS)

LDSCRIPT   ?= $(VARIANT_DIR)/linker_scripts/gcc/flash_with_bootloader.ld
LLDFLAGS   := $(CPUFLAGS) --specs=nano.specs --specs=nosys.specs
LLDFLAGS   += $(if $(filter $(PRINTF_FLOAT),1), $(PRINTF_FLOAT_FLAG))
LLDFLAGS   += -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all
LLDFLAGS   += -Wl,--warn-common -Wl,--warn-section-align
LLDFLAGS   += -Wl,-Map=$(OBJDIR)/$(TARGET).map
LLDFLAGS   += -static -L$(OBJDIR) -L$(CMSIS_DIR) -Wl,--as-needed

LIBS       := -l:arduino_libraries_lib.a -l:arduino_core_lib.a

define override_flags
override $(1) := $$(strip $$(L$(1)) $$($(1)))
endef
$(foreach f,CPPFLAGS CFLAGS CXXFLAGS ASFLAGS LDFLAGS LIBS,$(eval $(call override_flags,$(f))))

TARGET_ELF  := $(OBJDIR)/$(TARGET).elf
TARGET_BIN  := $(OBJDIR)/$(TARGET).bin
TARGET_HEX  := $(OBJDIR)/$(TARGET).hex

V ?= 0
_V_CC_0        = @echo "  CC      " $<;
_V_CXX_0       = @echo "  CXX     " $<;
_V_AS_0        = @echo "  AS      " $<;
_V_LD_0        = @echo "  LD      " $@;
_V_AR_0        = @echo "  AR      " $@;
_V_BIN_0       = @echo "  BIN     " $@;
_V_HEX_0       = @echo "  HEX     " $@;
_V_SIZE_0      = @echo "Program Size:";
_V_RESET_0     = @echo "  RESET   " $(COMPORT);
_V_UPLOAD_0    = @echo "  UPLOAD  " $<;
_V_CLEAN_0     = @echo "  CLEAN";

.PHONY: all
all: $(TARGET_BIN) .size_done

SIZE_CMD = $(_V_SIZE_$(V))scripts/print-size.py -q $(TARGET_ELF)
.PHONY: size
size: $(TARGET_ELF) | $(TARGET_BIN)
	$(SIZE_CMD)

.size_done: $(TARGET_ELF) | $(TARGET_BIN)
	$(SIZE_CMD)
	@touch $@

.PHONY: upload
upload: $(TARGET_BIN) all
	#$(_V_RESET_$(V))$(RESET_SCRIPT)
	$(_V_UPLOAD_$(V))$(BOSSAC) $(BOSSAC_FLAGS) $<

.PHONY: clean
clean:
	$(_V_CLEAN_$(V))rm -rf $(OBJDIR) .size_done

.PHONY: gdb
gdb: $(TARGET_ELF)
	$(GDB) -q $(TARGET_ELF) -ex "target extended-remote :2331" -ex "load" -ex "mon reset"

.PHONY: dis
dis: $(TARGET_ELF)
	@$(OBJDUMP) -dC $(TARGET_ELF)

.PHONY: disvim
disvim: $(TARGET_ELF)
	$(OBJDUMP) -dC $(TARGET_ELF) | vi -R -c ':set ft=asm' -

.PHONY: hex
hex: $(TARGET_HEX)

# Create static libraries of Arduino source code and libraries
$(OBJDIR)/arduino_core_lib.a: CXXFLAGS += $(CXXFLAGS_EXTRA_ARDUINO)
$(OBJDIR)/arduino_core_lib.a: CFLAGS += $(CFLAGS_EXTRA_ARDUINO)
$(OBJDIR)/arduino_core_lib.a: $(CORE_OBJS)
	$(_V_AR_$(V))$(AR) -r -o $@ $^

$(OBJDIR)/arduino_libraries_lib.a: CXXFLAGS += $(CXXFLAGS_EXTRA_ARDUINO)
$(OBJDIR)/arduino_libraries_lib.a: $(LIBRARIES_OBJS)
	$(_V_AR_$(V))$(AR) -r -o $@ $^

# Exception for Arduino SD library as SD.h includes `Print.h` and not `api/Print.h`
$(OBJDIR)/lib/SD/src/%.o: CPPFLAGS += -I$(CORE_API_DIR)api
$(OBJDIR)/src/music/%.o: CPPFLAGS += -I$(CORE_API_DIR)api

# Generic targets
$(SRCS): Makefile

$(TARGET_ELF): $(OBJDIR)/arduino_core_lib.a $(OBJDIR)/arduino_libraries_lib.a $(TARGET_OBJ) $(VARIANT_OBJ) $(LDSCRIPT)
	$(_V_LD_$(V))$(CXXLD) $(LDFLAGS) -T$(LDSCRIPT) -o $@ $(TARGET_OBJ) $(VARIANT_OBJ) $(LIBS)

$(TARGET_BIN): $(TARGET_ELF)
	$(_V_BIN_$(V))$(OBJCOPY) -O binary $< $@

$(TARGET_HEX): $(TARGET_ELF)
	$(_V_HEX_$(V))$(OBJCOPY) -O ihex $< $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(abspath $(dir $@))
	$(_V_CC_$(V))$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(abspath $(dir $@))
	$(_V_CXX_$(V))$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

-include $(patsubst %.o, %.d,$(SRCS))
