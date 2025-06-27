#!/usr/bin/env python3

import os
import argparse
import subprocess
import re
from pathlib import Path
# 彩色打印支持
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_info(message):
    print(f"{Colors.OKBLUE}[信息]{Colors.ENDC} {message}")

def print_success(message):
    print(f"{Colors.OKGREEN}[成功]{Colors.ENDC} {message}")

def print_warning(message):
    print(f"{Colors.WARNING}[警告]{Colors.ENDC} {message}")

def print_error(message):
    print(f"{Colors.FAIL}[错误]{Colors.ENDC} {message}")

def convert_mp3_to_opus(mp3_file, sample_rate=16000, channels=1, bitrate=16):
    """
    使用ffmpeg将MP3文件转换为Opus格式
    
    参数:
        mp3_file (str): 输入MP3文件路径
        sample_rate (int): 目标采样率
        channels (int): 目标声道数
        bitrate (int): Opus比特率(kbps)
    
    返回:
        bytes: Opus数据
    """
    try:
        # 创建临时文件
        temp_opus_file = os.path.join(os.path.dirname(mp3_file), f"temp_{os.path.basename(mp3_file)}.opus")
        
        # 使用ffmpeg转换MP3到Opus
        cmd = [
            'ffmpeg', '-y', '-i', mp3_file, 
            '-c:a', 'libopus',  # 使用libopus编码器
            '-ar', str(sample_rate),
            '-ac', str(channels),
            '-b:a', f'{bitrate}k',  # 设置比特率
            temp_opus_file
        ]
        
        subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        # 读取Opus数据
        with open(temp_opus_file, 'rb') as f:
            opus_data = f.read()
        
        # 删除临时文件
        os.remove(temp_opus_file)
        
        return opus_data
    except subprocess.CalledProcessError as e:
        print_error(f"错误: ffmpeg转换失败 - {e.stderr.decode()}")
        return None
    except FileNotFoundError:
        print_error("错误: 未找到ffmpeg，请确保已安装ffmpeg")
        return None

def clean_filename(filename):
    """
    清理文件名，移除不适合作为C变量名的字符
    
    参数:
        filename (str): 文件名
    
    返回:
        str: 清理后的名称
    """
    # 移除扩展名
    name_without_ext = os.path.splitext(filename)[0]
    
    # 替换所有非字母数字和下划线的字符为下划线
    clean_name = re.sub(r'[^a-zA-Z0-9_]', '_', name_without_ext)
    
    # 确保名称以字母或下划线开头
    if not clean_name or not (clean_name[0].isalpha() or clean_name[0] == '_'):
        clean_name = '_' + clean_name
    
    return clean_name

def batch_mp3_to_opus_header(input_folder, output_file="tone.h", elements_per_line=200, sample_rate=16000, channels=1, bitrate=16):
    """
    批量将MP3文件转换为Opus格式的C语言头文件
    
    参数:
        input_folder (str): 输入MP3文件夹路径
        output_file (str): 输出头文件路径
        elements_per_line (int): 每行包含的元素数
        sample_rate (int): 采样率
        channels (int): 声道数
        bitrate (int): Opus比特率(kbps)
    """
    # 确保输入文件夹存在
    if not os.path.exists(input_folder):
        print_error(f"错误: 找不到文件夹 '{input_folder}'")
        return False
    
    # 获取所有MP3文件
    mp3_files = [f for f in os.listdir(input_folder) if f.lower().endswith('.mp3')]
    
    if not mp3_files:
        print_error(f"错误: 在文件夹 '{input_folder}' 中找不到MP3文件")
        return False
    
    print_success(f"找到 {len(mp3_files)} 个MP3文件")
    
    # 生成头文件内容
    header_content = []
    header_content.append(f"#ifndef _TONE_H_")
    header_content.append(f"#define _TONE_H_")
    header_content.append(f"")
    header_content.append(f"// 音频参数")
    header_content.append(f"#define TONE_SAMPLE_RATE {sample_rate}")
    header_content.append(f"#define TONE_CHANNELS {channels}")
    header_content.append(f"#define TONE_BITRATE {bitrate}")
    header_content.append(f"")
    
    success_count = 0
    failed_count = 0
    
    # 处理每个MP3文件
    for i, mp3_file in enumerate(mp3_files):
        mp3_path = os.path.join(input_folder, mp3_file)
        print_info(f"处理文件 {i+1}/{len(mp3_files)}: {mp3_file}")
        
        # 生成C数组名
        array_name = clean_filename(mp3_file)
        
        # print(f"正在将MP3文件转换为 {sample_rate}Hz 采样率, {channels} 声道, {bitrate}kbps的Opus数据...")
        opus_data = convert_mp3_to_opus(mp3_path, sample_rate, channels, bitrate)
        
        if opus_data is None:
            failed_count += 1
            continue
        
        # print(f"  Opus数据转换成功，大小: {len(opus_data)} 字节")
        
        # 添加数组定义到头部内容
        header_content.append(f"// {mp3_file} - {len(opus_data)} bytes")
        header_content.append(f"const unsigned char {array_name}[] = {{")
        
        # 将二进制数据转换为16进制格式
        hex_values = []
        for byte in opus_data:
            hex_values.append(f"0x{byte:02X}")
        
        # 格式化数组内容，每行指定数量的元素
        for j in range(0, len(hex_values), elements_per_line):
            line = ", ".join(hex_values[j:j+elements_per_line])
            if j + elements_per_line < len(hex_values):
                header_content.append(f"    {line},")
            else:
                header_content.append(f"    {line}")
        
        header_content.append(f"}};")
        header_content.append(f"const unsigned int {array_name}_size = {len(opus_data)};")
        header_content.append(f"")
        
        success_count += 1
    
    # 添加文件结束部分
    header_content.append(f"#endif // _TONE_H_")
    
    # 写入输出文件
    with open(output_file, 'w') as f:
        f.write("\n".join(header_content))
    
    print_success(f"转换完成: 成功={success_count}, 失败={failed_count}, 总数={len(mp3_files)}")
    print_success(f"头文件已保存至: '{output_file}',可以使用 \033[93m cp tone.h ../doubao/lib/axk_app \033[0m替换源文件")
    
    return success_count > 0

if __name__ == "__main__":
    # 设置命令行参数解析
    parser = argparse.ArgumentParser(description="将文件夹中的MP3文件批量转换为16kHz采样率、单声道的Opus格式C语言头文件")
    parser.add_argument("input_folder", help="输入的MP3文件夹路径")
    parser.add_argument("-o", "--output", default="tone.h", help="输出的头文件路径 (默认: tone.h)")
    parser.add_argument("-e", "--elements_per_line", type=int, default=200, help="每行元素数 (默认: 200)")
    parser.add_argument("-r", "--rate", type=int, default=16000, help="采样率 (默认: 16000Hz)")
    parser.add_argument("-c", "--channels", type=int, default=1, help="声道数 (默认: 1=单声道)")
    parser.add_argument("-b", "--bitrate", type=int, default=16, help="Opus比特率(kbps) (默认: 16kbps)")
    
    # 解析参数
    args = parser.parse_args()
    
    # 执行转换
    batch_mp3_to_opus_header(args.input_folder, args.output, args.elements_per_line, args.rate, args.channels, args.bitrate)
