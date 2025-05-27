#!/bin/bash

# 获取系统类型 (Linux/Darwin)
SYSTEM_TYPE=$(uname -s)
echo "当前系统类型: $SYSTEM_TYPE"
git submodule init
# 检查os文件夹是否为空
if [ -z "$(ls -A os 2>/dev/null)" ]; then
    echo "SDK为空,正在拉取..."
	git submodule update --remote os

    sleep 2
    
    # 判断拉取是否成功
    if [ $? -ne 0 ]; then
        echo "错误：拉取失败！"
        exit 1
    fi
fi

if [ "$YSYTEM_TYPE"="Linux" ]; then
	if [ -z "$(ls -A toolchains/linux 2>/dev/null)" ]; then
		echo "检测到toolchain为空,开始系统适配..."
		echo "正在配置Linux工具链..."
		git submodule update --remote toolchains/linux
		exit 0
	else
		echo "Linux "riscv64-unknown-elf-" 工具链已存在"
		exit 0
	fi
fi 
# 这里添加Windows专用操作
if [ "$YSYTEM_TYPE"="Windows" ] ; then
	if [ -z "$(ls -A toolchains/windows 2>/dev/null)" ]; then
		echo "正在配置Windows工具链..."
		git submodule update --remote toolchains/windos
		exit 0
	else
		echo "Windows \"riscv64-unknown-elf-\" 工具链已存在"
		exit 0
	fi
else
	echo "错误：不支持的系统类型 $SYSTEM_TYPE"
	exit 1
fi

echo "操作执行完毕"
