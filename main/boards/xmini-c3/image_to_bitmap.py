#!/usr/bin/env python3
"""
图片转位图数组工具
将128x64像素的图片转换为SSD1306兼容的1位位图数组

支持格式：
- RGB/RGBA图片：根据亮度阈值转换为黑白
- 1位位图：直接转换

输出：
- C语言数组格式
- 符合SSD1306垂直排列格式
"""

import sys
import os
import re
from PIL import Image
import numpy as np

def natural_sort_key(filename):
    """
    生成自然排序的键值，正确处理数字序列
    例如: frame1.png, frame2.png, frame10.png 会按正确顺序排序
    """
    # 提取文件名中的数字部分
    parts = re.split(r'(\d+)', filename)
    # 将数字部分转换为整数，非数字部分保持字符串
    return [int(part) if part.isdigit() else part.lower() for part in parts]

def rgb_to_brightness(r, g, b):
    """将RGB值转换为亮度值 (0-255)"""
    # 使用标准亮度公式
    return int(0.299 * r + 0.587 * g + 0.114 * b)

def image_to_bitmap_array(image_path, threshold=128, output_file=None, save_preview=True, parent_folder=None, frame_index=None, reverse=False):
    """
    将图片转换为位图数组
    
    Args:
        image_path: 输入图片路径
        threshold: 亮度阈值 (0-255)
        output_file: 输出文件路径，如果为None则打印到控制台
        save_preview: 是否保存转换后的单色预览图片
        parent_folder: 父文件夹名称，用于命名规则
        frame_index: 帧索引，用于命名规则
        reverse: 阈值比较逻辑，False时大于threshold为白色，True时小于threshold为白色
    
    Returns:
        str: C语言数组字符串
    
    Note:
        生成的C数组数据会进行水平和垂直翻转以修正SSD1306显示翻转问题，
        但预览图片保持原始方向不变
    """
    
    # 打开图片
    try:
        img = Image.open(image_path)
    except Exception as e:
        raise ValueError(f"无法打开图片 {image_path}: {e}")
    
    # 检查尺寸
    if img.size != (128, 64):
        raise ValueError(f"图片尺寸必须是128x64，当前尺寸为{img.size}")
    
    # 转换为RGB模式
    if img.mode != 'RGB':
        img = img.convert('RGB')
    
    # 转换为numpy数组
    img_array = np.array(img)
    
    # 计算每个像素的亮度
    brightness = np.zeros((64, 128), dtype=np.uint8)
    for y in range(64):
        for x in range(128):
            r, g, b = img_array[y, x]
            brightness[y, x] = rgb_to_brightness(r, g, b)
    
    # 根据阈值转换为黑白
    if reverse:
        # reverse=True时，小于threshold的像素为白色
        binary = (brightness < threshold).astype(np.uint8)
    else:
        # reverse=False时，大于threshold的像素为白色
        binary = (brightness > threshold).astype(np.uint8)
    
    # 保存预览图片
    if save_preview:
        # 获取输入文件的目录
        input_dir = os.path.dirname(image_path)
        
        # 创建preview目录
        preview_dir = os.path.join(input_dir, 'preview')
        os.makedirs(preview_dir, exist_ok=True)
        
        # 生成预览图片路径，命名规则为%parent_folder%_%frame_index%.png
        if parent_folder is not None and frame_index is not None:
            preview_filename = f"{parent_folder}_{frame_index}.png"
        else:
            # 如果没有提供parent_folder和frame_index，使用原来的命名方式
            input_filename = os.path.splitext(os.path.basename(image_path))[0]
            preview_filename = f"preview_{input_filename}.png"
        
        preview_path = os.path.join(preview_dir, preview_filename)
        preview_img = Image.fromarray((binary * 255).astype(np.uint8), mode='L')
        preview_img.save(preview_path)
        print(f"预览图片已保存: {preview_path}")
    
    # 转换为SSD1306格式 (垂直排列，8像素为1字节)
    # 为了修正SSD1306显示翻转问题，对图像进行水平和垂直翻转
    binary_flipped = np.fliplr(binary)  # 水平翻转（左右翻转）
    binary_flipped = np.flipud(binary_flipped)  # 垂直翻转（上下翻转）
    
    bitmap_data = []
    
    for page in range(8):  # 8页，每页8行
        for col in range(128):  # 128列
            byte_value = 0
            for bit in range(8):  # 每字节8位
                y = page * 8 + bit
                if y < 64 and binary_flipped[y, col]:
                    byte_value |= (1 << bit)
            bitmap_data.append(byte_value)
    
    # 生成C语言数组字符串
    if parent_folder is not None and frame_index is not None:
        array_name = f"{parent_folder}_{frame_index}"
    else:
        # 如果没有提供parent_folder和frame_index，使用原来的命名方式
        array_name = os.path.splitext(os.path.basename(image_path))[0]
        array_name = array_name.replace('-', '_').replace(' ', '_')
    
    c_array = f"// {array_name} - 128x64像素，1位单色位图\n"
    c_array += f"const uint8_t {array_name}[] = {{\n"
    
    # 每行16个字节
    for i in range(0, len(bitmap_data), 16):
        line_bytes = bitmap_data[i:i+16]
        hex_values = [f"0x{b:02X}" for b in line_bytes]
        c_array += f"    {', '.join(hex_values)},\n"
    
    c_array += "};\n"
    
    # 输出到文件或控制台
    if output_file:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(c_array)
        print(f"位图数组已保存到: {output_file}")
    else:
        print(c_array)
    
    return c_array

def batch_convert_images(input_dir, output_file, threshold=128, save_preview=True, reverse=False):
    """
    批量转换目录中的所有图片
    
    Args:
        input_dir: 输入目录
        output_file: 输出文件
        threshold: 亮度阈值
        save_preview: 是否保存转换后的单色预览图片
        reverse: 阈值比较逻辑，False时大于threshold为白色，True时小于threshold为白色
    """
    
    supported_formats = ('.png', '.jpg', '.jpeg', '.bmp', '.gif')
    image_files = []
    
    # 查找所有支持的图片文件
    for filename in os.listdir(input_dir):
        if filename.lower().endswith(supported_formats):
            image_files.append(os.path.join(input_dir, filename))
    
    if not image_files:
        print(f"在目录 {input_dir} 中未找到支持的图片文件")
        return
    
    print(f"找到 {len(image_files)} 个图片文件")
    
    # 获取父文件夹名称
    # 规范化路径并移除末尾的路径分隔符
    normalized_dir = os.path.normpath(input_dir)
    parent_folder = os.path.basename(normalized_dir)
    
    # 生成完整的C文件
    c_content = '#include "emoji_data.h"\n\n'
    c_content += 'namespace bitmap_emoji {\n\n'
    
    # 使用自然排序确保动画帧序列正确
    sorted_image_files = sorted(image_files, key=lambda x: natural_sort_key(os.path.basename(x)))
    
    print("图片文件排序顺序:")
    for i, image_path in enumerate(sorted_image_files):
        print(f"  {i}: {os.path.basename(image_path)}")
    
    for frame_index, image_path in enumerate(sorted_image_files):
        print(f"转换: {os.path.basename(image_path)} (帧索引: {frame_index})")
        try:
            array_str = image_to_bitmap_array(image_path, threshold, None, save_preview, parent_folder, frame_index, reverse)
            c_content += array_str + '\n'
        except Exception as e:
            print(f"错误: {e}")
            continue
    
    c_content += '\n} // namespace bitmap_emoji\n'
    
    # 保存到文件
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(c_content)
    
    print(f"所有位图数组已保存到: {output_file}")

def create_test_patterns():
    """创建一些测试图案"""
    
    patterns = {
        'test_border': create_border_pattern(),
        'test_stripes': create_stripe_pattern(),
        'test_checker': create_checker_pattern(),
        'test_gradient': create_gradient_pattern()
    }
    
    for name, pattern in patterns.items():
        # 保存为PNG文件
        img = Image.fromarray((pattern * 255).astype(np.uint8), mode='L')
        img = img.convert('RGB')
        img.save(f"{name}.png")
        print(f"创建测试图案: {name}.png")

def create_border_pattern():
    """创建边框测试图案"""
    pattern = np.zeros((64, 128), dtype=np.uint8)
    # 边框
    pattern[0, :] = 1      # 顶部
    pattern[-1, :] = 1     # 底部
    pattern[:, 0] = 1      # 左侧
    pattern[:, -1] = 1     # 右侧
    return pattern

def create_stripe_pattern():
    """创建条纹测试图案"""
    pattern = np.zeros((64, 128), dtype=np.uint8)
    for y in range(64):
        if y % 4 < 2:  # 每4行中有2行为白色
            pattern[y, :] = 1
    return pattern

def create_checker_pattern():
    """创建棋盘测试图案"""
    pattern = np.zeros((64, 128), dtype=np.uint8)
    for y in range(64):
        for x in range(128):
            if (x + y) % 8 < 4:  # 8x8的棋盘
                pattern[y, x] = 1
    return pattern

def create_gradient_pattern():
    """创建渐变测试图案"""
    pattern = np.zeros((64, 128), dtype=np.uint8)
    for x in range(128):
        # 水平渐变，从左到右
        if x < 64:
            pattern[:, x] = 1
    return pattern

def main():
    """主函数"""
    if len(sys.argv) < 2:
        print("用法:")
        print("  python image_to_bitmap.py <图片路径> [阈值] [输出文件] [--no-preview] [--reverse]")
        print("  python image_to_bitmap.py --batch <输入目录> <输出文件> [阈值] [--no-preview] [--reverse]")
        print("  python image_to_bitmap.py --create-test")
        print("")
        print("参数:")
        print("  图片路径: 128x64像素的图片文件")
        print("  阈值: 亮度阈值 (0-255)，默认128")
        print("  输出文件: C语言数组输出文件，可选")
        print("  --no-preview: 不保存预览图片")
        print("  --reverse: 反转阈值逻辑，小于阈值的像素为白色")
        print("")
        print("示例:")
        print("  python image_to_bitmap.py happy.png")
        print("  python image_to_bitmap.py happy.png 100 happy_bitmap.txt")
        print("  python image_to_bitmap.py happy.png 100 happy_bitmap.txt --no-preview")
        print("  python image_to_bitmap.py happy.png 100 happy_bitmap.txt --reverse")
        print("  python image_to_bitmap.py --batch ./images/ emoji_data.cc")
        print("  python image_to_bitmap.py --batch ./images/ emoji_data.cc 128 --reverse")
        print("  python image_to_bitmap.py --create-test")
        return
    
    if sys.argv[1] == '--create-test':
        create_test_patterns()
        return
    
    if sys.argv[1] == '--batch':
        if len(sys.argv) < 4:
            print("批量转换用法: python image_to_bitmap.py --batch <输入目录> <输出文件> [阈值] [--no-preview]")
            return
        
        input_dir = sys.argv[2]
        output_file = sys.argv[3]
        threshold = 128
        save_preview = True
        reverse = False
        
        # 解析参数
        for i in range(4, len(sys.argv)):
            if sys.argv[i] == '--no-preview':
                save_preview = False
            elif sys.argv[i] == '--reverse':
                reverse = True
            else:
                try:
                    threshold = int(sys.argv[i])
                except ValueError:
                    pass
        
        batch_convert_images(input_dir, output_file, threshold, save_preview, reverse)
        return
    
    # 单文件转换
    image_path = sys.argv[1]
    threshold = 128
    output_file = None
    save_preview = True
    reverse = False
    
    # 解析参数
    for i in range(2, len(sys.argv)):
        if sys.argv[i] == '--no-preview':
            save_preview = False
        elif sys.argv[i] == '--reverse':
            reverse = True
        elif output_file is None and not sys.argv[i].startswith('--'):
            try:
                threshold = int(sys.argv[i])
            except ValueError:
                output_file = sys.argv[i]
    
    try:
        image_to_bitmap_array(image_path, threshold, output_file, save_preview, None, None, reverse)
    except Exception as e:
        print(f"错误: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
