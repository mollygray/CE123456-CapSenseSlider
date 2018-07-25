#
# Demonstrates the PSoC 6 CapSense slider and CapSense button
#

#
# Toolchain, its optimization level and the configuration type
#
TOOLCHAIN=GCC
OPTIMIZATION = Og
CONFIG = DEBUG

ifeq ($(TOOLCHAIN),GCC)
VFP_FLAG = -mfloat-abi=softfp -mfpu=fpv4-sp-d16
else
ifeq ($(TOOLCHAIN),IAR)
VFP_FLAG = --fpu VFPv4_sp
else
ifeq ($(TOOLCHAIN),MDK)
VFP_FLAG = --fpu=softvfp+fpv4-sp
endif
endif
endif

#
# The target platform for the example
#
PLATFORMS_VERSION=1.0
PLATFORM=PSOC6_DUAL_CORE
DEVICE=CY8C6347BZI-BLD53

#
# The default name of this example
#
CY_EXAMPLE_NAME = CapSenseSlider

#
# Description of the example project to display
#
CY_EXAMPLE_DESCRIPTION = Dual core PSoC6 capsense slider application

#
# New project dialog inclusion
#
CY_SHOW_NEW_PROJECT = true

#
# Valid platforms for this example
#
CY_VALID_PLATFORMS = PSOC6_DUAL_CORE

#
# This is the required SDK for this example
#
CY_REQUIRED_SDK = Cypress SDK[1.0]

#
# Valid devices for this example. If empty, this example works for all devices
#
CY_VALID_DEVICES =

#
# Source codes for the CM0+ application
#
CY_APP_CM0P_SOURCE = \
	prj_CM0p/main.c\
	prj_CM0p/cycfg_init.c\
	prj_CM0p/cy_capsense_config.h

#
# Source codes for the CM4 application
#
CY_APP_CM4_SOURCE = \
	prj_CM4/main.c\
	prj_CM4/cy_capsense_config.h

#
# Includes specific to the CM0+ application
#
APP_MAINAPP_CM0P_INCLUDES = \
	-IGeneratedSource\
	-Iprj_CM0p\
	-I$(CY_GENERATED_DIR)/$(CYMAINAPP_CM0P_NAME)/prj_CM0p

#
# Includes specific to the CM4 application
#
APP_MAINAPP_CM4_INCLUDES = \
	-IGeneratedSource\
	-Iprj_CM4\
	-I$(CY_GENERATED_DIR)/$(CYMAINAPP_CM4_NAME)/prj_CM4

#
# Compiler flags specific to the CM0+ application
#
APP_MAINAPP_CM0P_FLAGS =

#
# Compiler flags specific to the CM4 application
#
APP_MAINAPP_CM4_FLAGS = \
	$(VFP_FLAG)

#
# Defines specific to the CM0+ application
#
APP_MAINAPP_CM0P_DEFINES = \
	-DAPP_NAME='"$(CY_EXAMPLE_NAME)_cm0p"'\
	-D$(CONFIG)

#
# Defines specific to the CM4 application
#
APP_MAINAPP_CM4_DEFINES = \
	-DAPP_NAME='"$(CY_EXAMPLE_NAME)_cm4"'\
	-D$(CONFIG)

#
# Software (middleware) components needed by CM0+
#
CY_MAINAPP_CM0P_SWCOMP_USED = \
	$(CY_PSOC_LIB_COMP_MIDDLEWARE_BASE)/capsense/softfp

#
# Software (middleware) components needed by CM4
#
CY_MAINAPP_SWCOMP_USED = \
	$(CY_PSOC_LIB_COMP_MIDDLEWARE_BASE)/capsense/softfp

#
# Other libraries (.a) needed by the CM0+ application
#
APP_MAINAPP_CM0P_LIBS = \

#
# Other libraries (.a) needed by the CM4 application
#
APP_MAINAPP_CM4_LIBS = \

#
# The path to the design.modus file
#
CYCONFIG_DESIGN_MODUS = design.modus

#
# The set of generated source files
#
CYCONFIG_GENERATED_SOURCES = \
	GeneratedSource/cycfg_capsense.c\
	GeneratedSource/cycfg_capsense.h

#
# Check to be sure we can find the SDK.  The SDK is found via the
# CYSDK environment variable
#
ifndef CYSDK
$(error The SDK must be defined via the CYSDK environment variable)
endif

#
# Include the main makefile for building this type of example
#
include $(CYSDK)/libraries/platforms-$(PLATFORMS_VERSION)/$(PLATFORM).mk
