##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -O2 -ggdb -fomit-frame-pointer -falign-functions=16
  USE_OPT += -frename-registers -freorder-blocks -fconserve-stack -fstack-usage
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = -std=gnu99
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = --std=c++14
  USE_CPPOPT += -fno-rtti -fno-exceptions
endif

# Enable this if you want the linker to remove unused code and data
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT = -lm
endif

# Enable this if you want link time optimizations (LTO)
ifeq ($(USE_LTO),)
  USE_LTO = no
endif

# If enabled, this option allows to compile the application in THUMB mode.
ifeq ($(USE_THUMB),)
  USE_THUMB = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = no
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x400
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x400
endif

# Enables the use of FPU (no, softfp, hard).
ifeq ($(USE_FPU),)
  USE_FPU = hard
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, sources and paths
#

# Define project name here
PROJECT = hydrocontest

# Imported source files and paths
CHIBIOS = lib/ChibiOS

# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk
# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F4xx/platform.mk
include $(CHIBIOS)/os/hal/osal/rt/osal.mk
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/port_v7m.mk
# Other files (optional).
include $(CHIBIOS)/os/hal/lib/streams/streams.mk
include $(CHIBIOS)/os/various/shell/shell.mk

# Define linker script file here
LDSCRIPT= src/platform/pixracer/STM32F427xI.ld

include app_src.mk

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC += $(STARTUPSRC) \
        $(KERNSRC) \
        $(PORTSRC) \
        $(OSALSRC) \
		$(SHELLSRC) \
        $(HALSRC) \
        $(PLATFORMSRC) \
		$(CHIBIOS)/os/hal/lib/streams/chprintf.c \
		$(CHIBIOS)/os/various/evtimer.c \
		$(CHIBIOS)/os/various/syscalls.c \
        $(BOARDSRC)

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC +=

# C sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACSRC +=

# C++ sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACPPSRC +=

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCSRC +=

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCPPSRC +=

# List ASM source files here
ASMSRC +=
ASMXSRC += $(STARTUPASM) $(PORTASM) $(OSALASM)

INCDIR += $(CHIBIOS)/os/license \
		  $(CHIBIOS)/os/various \
		  $(CHIBIOS)/os/hal/lib/streams \
          $(STARTUPINC) $(KERNINC) $(PORTINC) $(OSALINC) \
          $(HALINC) $(PLATFORMINC) $(BOARDINC) $(SHELLINC)

# PROTOSRC = $(wildcard messages/*.proto)
# CSRC += $(addprefix build/,$(PROTOSRC:.proto=.pb.c))

#
# Project, sources and paths
##############################################################################

##############################################################################
# Compiler settings
#

MCU  = cortex-m4

#TRGT = arm-elf-
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
# Enable loading with g++ only if you need C++ runtime support.
# NOTE: You can use C++ even without C++ support if you are careful. C++
#       runtime support makes code size explode.
LD   = $(TRGT)gcc
#LD   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
AR   = $(TRGT)ar
OD   = $(TRGT)objdump
SZ   = $(TRGT)size
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary

# ARM-specific options here
AOPT =

# THUMB-specific options here
TOPT = -mthumb -DTHUMB

# Define C warning options here
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes

# Define C++ warning options here
CPPWARN = -Wall -Wextra -Wundef

#
# Compiler settings
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS = -DSHELL_MAX_ARGUMENTS=5 \
	-DSHELL_CMD_TEST_ENABLED=FALSE \
	-DSHELL_CMD_EXIT_ENABLED=FALSE \
	-DSHELL_CMD_INFO_ENABLED=FALSE \


# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

#
# End of user defines
##############################################################################


RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC
include $(RULESPATH)/rules.mk

protoc:
	@echo Compiling protocol buffer files
	@mkdir -p build/messages
	@protoc \
		--plugin=protoc-gen-nanopb=lib/nanopb/nanopb/generator/protoc-gen-nanopb \
		--python_out=build/messages \
		-I lib/nanopb/nanopb/generator/proto \
		-I messages/ --nanopb_out=--library-include-format='#include\ \"%s\"':build/messages/ $(PROTOSRC)

# Generates a ctags file containing the correct definition for the build
.PHONY: ctags
ctags:
	@echo "Generating ctags file..."
	@cat .dep/*.d | grep ":$$" | sed "s/://" | sort | uniq | xargs ctags --file-scope=no --extra=+qf $(CSRC) $(CPPSRC)

.PHONY: mem_info
mem_info: $(BUILDDIR)/$(PROJECT).elf
	@arm-none-eabi-nm --size-sort --print-size $(BUILDDIR)/$(PROJECT).elf > $(BUILDDIR)/memory_size.txt
	@arm-none-eabi-nm --numeric-sort --print-size $(BUILDDIR)/$(PROJECT).elf > $(BUILDDIR)/memory_layout.txt
	@tools/heap_size.py $(BUILDDIR)/$(PROJECT).elf

all: mem_info

format:
	clang-format -style=file -i `git ls-tree --full-tree -r HEAD | grep -e "\.h$$" | cut -f 2`
	clang-format -style=file -i `git ls-tree --full-tree -r HEAD | grep -e "\.c$$" | cut -f 2`
	clang-format -style=file -i `git ls-tree --full-tree -r HEAD | grep -e "\.cpp$$" | cut -f 2`
	clang-format -style=file -i `git ls-tree --full-tree -r HEAD | grep -e "\.proto$$" | cut -f 2`

flash: $(BUILDDIR)/$(PROJECT).elf
	openocd -f "interface/stlink-v2-1.cfg" \
			-f "target/stm32f4x.cfg" \
			-c "program $(BUILDDIR)/$(PROJECT).elf verify reset exit"
