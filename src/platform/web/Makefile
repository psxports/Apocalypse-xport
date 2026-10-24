PROJECT_ROOT := ../../..
SHORT_NAME := A
FULL_NAME := Apocalypse
APP_DEFINES := -DWND_TITLE=\"Apocalypse\" -DWND_WIDTH=960 -DWND_HEIGHT=720 -DFIELD_RATE=60 -DXPORT_AUDIO_VBLANK_QUEUE=1 -DMDEC_CUSTOM=1 -DXPORT_NATIVE_VAB=1 -DXPORT_NATIVE_SEQUENCE=1 -DXPORT_NATIVE_VM=1 -DXPORT_NATIVE_CD=1 -DXPORT_NATIVE_CALLBACK_BOOTSTRAP=1 -DXPORT_DISABLE_FUNCTION_MARKERS
WEB_PACKAGER_FLAGS := --include-str DATA/WDN9A.STR
include app_sources.mk
APP_SOURCES +=
CXX_FLAGS := -std=c++98 -fms-extensions
include ../../../../xport/src/platform/web/Makefile

# The native VAB decoder stages more than 64 KiB on the stack
LINK_FLAGS += -sSTACK_SIZE=1048576 -sSTACK_OVERFLOW_CHECK=2
# Credits suspend below the main menu and its retained HD frame snapshots
LINK_FLAGS += -sASYNCIFY_STACK_SIZE=16384

ifeq ($(CONFIG),Debug)
# Unoptimized indirect calls require twenty parameters in Binaryen's cast adapter
LINK_FLAGS += -sBINARYEN_EXTRA_PASSES=--pass-arg=max-func-params@20
endif
