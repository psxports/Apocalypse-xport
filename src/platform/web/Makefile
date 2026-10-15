PROJECT_ROOT := ../../..
SHORT_NAME := A
FULL_NAME := Apocalypse
APP_DEFINES := -DWND_TITLE=\"Apocalypse\" -DWND_WIDTH=960 -DWND_HEIGHT=720 -DFIELD_RATE=50 -DMDEC_CUSTOM=1 -DXPORT_DISABLE_FUNCTION_MARKERS
WEB_PACKAGER_FLAGS := --extra-file $(PROJECT_ROOT)/orig/SLUS_003.73 orig/SLUS_003.73
include app_sources.mk
APP_SOURCES += ../../../../xport/src/psx_seq.c
include ../../../../xport/src/platform/web/Makefile
CPP_OBJECTS := $(foreach source,$(filter %.cpp,$(APP_SOURCES)),$(call object_for,$(source)))
$(CPP_OBJECTS): COMMON_FLAGS := $(filter-out -std=c11,$(COMMON_FLAGS)) -std=c++98 -fms-extensions
