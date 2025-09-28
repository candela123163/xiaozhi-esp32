#!/usr/bin/env python3
"""
图片转位图工具使用示例
演示如何使用image_to_bitmap.py工具
"""

import os
import sys
import math
from PIL import Image, ImageDraw
import numpy as np

def create_sample_images():
    """创建一些示例图片用于演示"""
    
    # 创建示例目录
    os.makedirs("sample_images", exist_ok=True)
    os.makedirs("sample_images/anim_emoji", exist_ok=True)
    
    # 1. 创建简单笑脸
    create_simple_smile()
    
    # 2. 创建悲伤脸
    create_sad_face()
    
    # 3. 创建思考脸
    create_thinking_face()
    
    # 4. 创建动态动画
    create_animation_samples()
    
    print("示例图片已创建在 sample_images/ 目录中")

def create_simple_smile():
    """创建简单笑脸"""
    img = Image.new('RGB', (128, 64), color='white')
    draw = ImageDraw.Draw(img)
    
    # 画圆形脸
    draw.ellipse([20, 10, 108, 54], outline='black', width=2)
    
    # 画眼睛
    draw.ellipse([35, 20, 45, 30], fill='black')  # 左眼
    draw.ellipse([83, 20, 93, 30], fill='black')  # 右眼
    
    # 画嘴巴 (微笑)
    draw.arc([40, 35, 88, 50], start=0, end=180, fill='black', width=2)
    
    img.save("sample_images/simple_smile.png")
    print("创建: simple_smile.png")

def create_sad_face():
    """创建悲伤脸"""
    img = Image.new('RGB', (128, 64), color='white')
    draw = ImageDraw.Draw(img)
    
    # 画圆形脸
    draw.ellipse([20, 10, 108, 54], outline='black', width=2)
    
    # 画眼睛
    draw.ellipse([35, 20, 45, 30], fill='black')  # 左眼
    draw.ellipse([83, 20, 93, 30], fill='black')  # 右眼
    
    # 画嘴巴 (悲伤)
    draw.arc([40, 45, 88, 60], start=180, end=360, fill='black', width=2)
    
    img.save("sample_images/simple_sad.png")
    print("创建: simple_sad.png")

def create_thinking_face():
    """创建思考脸"""
    img = Image.new('RGB', (128, 64), color='white')
    draw = ImageDraw.Draw(img)
    
    # 画圆形脸
    draw.ellipse([20, 10, 108, 54], outline='black', width=2)
    
    # 画眼睛 (半闭)
    draw.arc([35, 22, 45, 28], start=0, end=180, fill='black', width=2)  # 左眼
    draw.arc([83, 22, 93, 28], start=0, end=180, fill='black', width=2)  # 右眼
    
    # 画嘴巴 (直线)
    draw.line([40, 40, 88, 40], fill='black', width=2)
    
    img.save("sample_images/simple_thinking.png")
    print("创建: simple_thinking.png")

def create_animation_samples():
    """创建动态动画示例"""
    print("\n创建动态动画示例...")
    
    # 创建心跳动画
    create_heartbeat_animation()
    
    # 创建眨眼动画
    create_blink_animation()
    
    print("动态动画创建完成！")

def create_heartbeat_animation():
    """创建8帧心跳动画"""
    frames = 8
    width, height = 128, 64
    center_x, center_y = width // 2, height // 2
    
    # 心跳动画的时间序列 - 增加更明显的差异
    heartbeat_sequence = [0.0, 0.2, 0.6, 1.0, 0.9, 0.5, 0.2, 0.0]
    
    for frame in range(frames):
        img = Image.new('RGB', (width, height), color='white')
        draw = ImageDraw.Draw(img)
        
        intensity = heartbeat_sequence[frame]
        base_size = 15  # 减小基础大小，让变化更明显
        current_size = int(base_size * (1 + intensity * 1.2))  # 增加变化幅度
        
        # 绘制心形
        draw_heart(draw, center_x, center_y, current_size, intensity)
        
        # 添加心跳线
        if intensity > 0.1:
            draw_heartbeat_line(draw, center_x, center_y, intensity, frame)
        
        # 添加心跳波纹效果
        if intensity > 0.3:
            draw_heartbeat_ripples(draw, center_x, center_y, intensity, frame)
        
        filename = f"sample_images/anim_emoji/heartbeat_frame_{frame+1:02d}.png"
        img.save(filename)
        print(f"创建: heartbeat_frame_{frame+1:02d}.png (强度: {intensity:.1f}, 大小: {current_size})")

def draw_heart(draw, center_x, center_y, size, intensity):
    """绘制心形图案"""
    heart_width = size
    heart_height = int(size * 0.8)
    
    circle_radius = heart_width // 4
    left_circle_x = center_x - heart_width // 4
    right_circle_x = center_x + heart_width // 4
    top_y = center_y - heart_height // 2
    bottom_y = center_y + heart_height // 2
    
    # 根据强度调整线条粗细
    line_width = max(1, int(2 + intensity * 2))
    
    # 左半圆
    draw.ellipse([left_circle_x - circle_radius, top_y, 
                 left_circle_x + circle_radius, top_y + circle_radius * 2], 
                outline='black', width=line_width)
    
    # 右半圆
    draw.ellipse([right_circle_x - circle_radius, top_y, 
                 right_circle_x + circle_radius, top_y + circle_radius * 2], 
                outline='black', width=line_width)
    
    # 底部三角形
    triangle_points = [
        (left_circle_x, top_y + circle_radius),
        (right_circle_x, top_y + circle_radius),
        (center_x, bottom_y)
    ]
    draw.polygon(triangle_points, outline='black', width=line_width)
    
    # 高强度时填充
    if intensity > 0.4:
        fill_points = [
            (left_circle_x - circle_radius//2, top_y + circle_radius//2),
            (right_circle_x + circle_radius//2, top_y + circle_radius//2),
            (center_x, bottom_y - 3)
        ]
        draw.polygon(fill_points, fill='black')
    
    # 超高强度时添加内部细节
    if intensity > 0.8:
        # 添加内部小圆点
        inner_radius = circle_radius // 3
        draw.ellipse([center_x - inner_radius, center_y - inner_radius//2, 
                     center_x + inner_radius, center_y + inner_radius//2], 
                    fill='white', outline='black', width=1)

def draw_heartbeat_line(draw, center_x, center_y, intensity, frame):
    """绘制心跳线"""
    line_length = int(40 * intensity)  # 增加线条长度
    line_y = center_y + 25
    
    start_x = center_x - line_length // 2
    end_x = center_x + line_length // 2
    
    # 根据强度调整线条粗细
    line_width = max(1, int(1 + intensity * 3))
    
    if frame % 2 == 0:
        # 直线心跳
        draw.line([start_x, line_y, end_x, line_y], fill='black', width=line_width)
    else:
        # 波浪心跳
        points = []
        for i in range(0, line_length, 3):
            x = start_x + i
            wave_y = line_y + int(5 * math.sin(i * 0.3) * intensity)
            points.append((x, wave_y))
        
        if len(points) > 1:
            draw.line(points, fill='black', width=line_width)
    
    # 高强度时添加额外的装饰线
    if intensity > 0.6:
        extra_y = line_y + 8
        extra_length = int(20 * intensity)
        extra_start = center_x - extra_length // 2
        extra_end = center_x + extra_length // 2
        draw.line([extra_start, extra_y, extra_end, extra_y], fill='black', width=1)

def draw_heartbeat_ripples(draw, center_x, center_y, intensity, frame):
    """绘制心跳波纹效果"""
    # 绘制同心圆波纹
    ripple_count = int(2 + intensity * 2)
    
    for i in range(ripple_count):
        ripple_radius = 20 + i * 8
        ripple_intensity = intensity * (1 - i * 0.3)
        
        if ripple_intensity > 0.2:
            # 绘制波纹圆
            ripple_x = center_x - ripple_radius
            ripple_y = center_y - ripple_radius
            ripple_x2 = center_x + ripple_radius
            ripple_y2 = center_y + ripple_radius
            
            # 根据帧数决定是否绘制
            if (frame + i) % 2 == 0:
                draw.ellipse([ripple_x, ripple_y, ripple_x2, ripple_y2], 
                           outline='black', width=1)

def create_blink_animation():
    """创建8帧眨眼动画"""
    blink_sequence = [1.0, 1.0, 0.8, 0.4, 0.0, 0.4, 0.8, 1.0]
    
    width, height = 128, 64
    center_x, center_y = width // 2, height // 2
    
    for frame in range(8):
        img = Image.new('RGB', (width, height), color='white')
        draw = ImageDraw.Draw(img)
        
        eye_openness = blink_sequence[frame]
        draw_blink_face(draw, center_x, center_y, eye_openness)
        
        filename = f"sample_images/anim_emoji/blink_frame_{frame+1:02d}.png"
        img.save(filename)
        print(f"创建: blink_frame_{frame+1:02d}.png")

def draw_blink_face(draw, center_x, center_y, eye_openness):
    """绘制眨眼表情"""
    # 绘制圆形脸
    face_size = 40
    face_left = center_x - face_size // 2
    face_top = center_y - face_size // 2
    face_right = center_x + face_size // 2
    face_bottom = center_y + face_size // 2
    
    draw.ellipse([face_left, face_top, face_right, face_bottom], 
                outline='black', width=2)
    
    # 绘制眼睛
    eye_y = center_y - 8
    left_eye_x = center_x - 12
    right_eye_x = center_x + 12
    
    if eye_openness > 0.5:
        # 睁眼状态
        draw.ellipse([left_eye_x - 4, eye_y - 4, left_eye_x + 4, eye_y + 4], 
                    fill='black')
        draw.ellipse([right_eye_x - 4, eye_y - 4, right_eye_x + 4, eye_y + 4], 
                    fill='black')
    else:
        # 闭眼状态
        draw.arc([left_eye_x - 4, eye_y - 2, left_eye_x + 4, eye_y + 2], 
                start=0, end=180, fill='black', width=2)
        draw.arc([right_eye_x - 4, eye_y - 2, right_eye_x + 4, eye_y + 2], 
                start=0, end=180, fill='black', width=2)
    
    # 绘制嘴巴
    mouth_y = center_y + 8
    draw.arc([center_x - 8, mouth_y - 4, center_x + 8, mouth_y + 4], 
            start=0, end=180, fill='black', width=2)

def demonstrate_conversion():
    """演示转换过程"""
    
    print("\n=== 图片转位图演示 ===")
    
    # 导入转换函数
    sys.path.append('.')
    from image_to_bitmap import image_to_bitmap_array
    
    # 转换示例图片
    images = [
        ("sample_images/simple_smile.png", "happy_face", 120),
        ("sample_images/simple_sad.png", "sad_face", 120),
        ("sample_images/simple_thinking.png", "thinking_face", 120)
    ]
    
    # 转换动画帧（只转换第一帧作为示例）
    animation_images = [
        ("sample_images/anim_emoji/heartbeat_frame_01.png", "heartbeat_frame1", 120),
        ("sample_images/anim_emoji/blink_frame_01.png", "blink_frame1", 120)
    ]
    
    print("\n生成的C语言数组:")
    print("=" * 50)
    
    # 转换静态表情
    for image_path, array_name, threshold in images:
        if os.path.exists(image_path):
            print(f"\n// 从 {image_path} 转换而来")
            try:
                array_str = image_to_bitmap_array(image_path, threshold, None, save_preview=True)
                # 替换数组名
                array_str = array_str.replace("simple_smile", array_name)
                array_str = array_str.replace("simple_sad", array_name)
                array_str = array_str.replace("simple_thinking", array_name)
                print(array_str)
            except Exception as e:
                print(f"转换失败: {e}")
        else:
            print(f"文件不存在: {image_path}")
    
    # 转换动画帧
    print("\n=== 动画帧转换示例 ===")
    for image_path, array_name, threshold in animation_images:
        if os.path.exists(image_path):
            print(f"\n// 从 {image_path} 转换而来")
            try:
                array_str = image_to_bitmap_array(image_path, threshold, None, save_preview=True)
                # 替换数组名
                array_str = array_str.replace("heartbeat_frame_01", array_name)
                array_str = array_str.replace("blink_frame_01", array_name)
                print(array_str)
            except Exception as e:
                print(f"转换失败: {e}")
        else:
            print(f"文件不存在: {image_path}")
    
    print("\n预览图片已生成，可以查看转换效果！")

def create_emoji_data_file():
    """创建完整的emoji_data.cc文件"""
    
    print("\n=== 创建emoji_data.cc文件 ===")
    
    # 生成完整的C文件内容
    c_content = '''#include "emoji_data.h"

namespace bitmap_emoji {

// 注意：以下数组需要从实际转换的图片生成
// 这里只是示例结构

// 开心表情 - 需要从实际图片转换
const uint8_t happy_face[EMOJI_DATA_SIZE] = {
    // 使用 python image_to_bitmap.py sample_images/simple_smile.png 120 生成
    // 然后将生成的数组复制到这里
};

// 悲伤表情 - 需要从实际图片转换  
const uint8_t sad_face[EMOJI_DATA_SIZE] = {
    // 使用 python image_to_bitmap.py sample_images/simple_sad.png 120 生成
    // 然后将生成的数组复制到这里
};

// 思考表情 - 需要从实际图片转换
const uint8_t thinking_face[EMOJI_DATA_SIZE] = {
    // 使用 python image_to_bitmap.py sample_images/simple_thinking.png 120 生成
    // 然后将生成的数组复制到这里
};

// 眨眼动画 - 5帧动画示例
const uint8_t blink_frame1[EMOJI_DATA_SIZE] = {
    // 全亮帧 - 睁眼状态
};

const uint8_t blink_frame2[EMOJI_DATA_SIZE] = {
    // 半闭眼帧
};

const uint8_t blink_frame3[EMOJI_DATA_SIZE] = {
    // 闭眼帧
};

const uint8_t blink_frame4[EMOJI_DATA_SIZE] = {
    // 半闭眼帧
};

const uint8_t blink_frame5[EMOJI_DATA_SIZE] = {
    // 全亮帧 - 睁眼状态
};

} // namespace bitmap_emoji
'''
    
    with open("emoji_data_template.cc", "w", encoding="utf-8") as f:
        f.write(c_content)
    
    print("已创建: emoji_data_template.cc")
    print("请按照注释中的说明生成实际的位图数据")

def main():
    """主函数"""
    print("图片转位图工具使用示例")
    print("=" * 40)
    
    # 创建示例图片
    create_sample_images()
    
    # 演示转换过程
    demonstrate_conversion()
    
    # 创建模板文件
    create_emoji_data_file()
    
    print("\n=== 使用说明 ===")
    print("1. 查看生成的示例图片:")
    print("   - 静态表情: sample_images/")
    print("   - 动态动画: sample_images/anim_emoji/")
    print("2. 使用转换工具:")
    print("   - 静态表情: python image_to_bitmap.py sample_images/simple_smile.png 120")
    print("   - 动画帧: python image_to_bitmap.py sample_images/anim_emoji/heartbeat_frame_01.png 120")
    print("3. 批量转换动画帧:")
    print("   python image_to_bitmap.py --batch sample_images/anim_emoji/ heartbeat_animation.cc")
    print("4. 将生成的数组复制到 emoji_data.cc 中")
    print("5. 在C代码中创建动画对象:")
    print("   EmojiFrame heartbeat_frames[8] = {")
    print("       EmojiFrame(heartbeat_frame1, 128, 64, 200),")
    print("       EmojiFrame(heartbeat_frame2, 128, 64, 200),")
    print("       // ... 其他帧")
    print("   };")
    print("   EmojiAnimation heartbeat_anim(heartbeat_frames, 8, true, 10);")
    print("6. 编译并测试")

if __name__ == "__main__":
    main()
