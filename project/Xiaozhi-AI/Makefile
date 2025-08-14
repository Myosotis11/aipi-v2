UNAME_S := $(shell uname -s)

# 根据系统类型设置变量
ifeq ($(UNAME_S),Linux)
    OS_TYPE = linux
else ifeq ($(findstring NT,$(UNAME_S)),NT)
    OS_TYPE = windows
endif

SDK_DEMO_PATH ?= $(shell pwd)

BL_SDK_BASE ?= $(SDK_DEMO_PATH)/../../os

export BL_SDK_BASE

NOW_REAL_PATH:= $(realpath ./../../)
CHIP ?= bl616
BOARD ?= aipi-palchatv2_board
BOARD_DIR ?= $(NOW_REAL_PATH)

CROSS_COMPILE ?= $(SDK_DEMO_PATH)/../../toolchains/$(OS_TYPE)/bin/riscv64-unknown-elf-

# add custom cmake definition
#cmake_definition+=-Dxxx=sss


include $(BL_SDK_BASE)/project.build
