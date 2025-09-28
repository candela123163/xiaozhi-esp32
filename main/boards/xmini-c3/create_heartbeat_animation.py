#!/usr/bin/env python3
"""
创建8帧心跳动画
生成心跳效果的动态图案，适合OLED显示
"""

import os
import math
from PIL import Image, ImageDraw
import numpy as np

def create_heartbeat_animation():
    """创建8帧心跳动画"""
    
    # 确保目录存在
    os.makedirs("sample_images/anim_emoji", exist_ok=True)
    
    # 动画参数
    frames = 8
    width, height = 128, 64
    center_x, center_y = width // 2, height // 2
    
    # 心跳动画的时间序列 (0-1之间的值，表示心跳的强度)
    heartbeat_sequence = [
        0.0,    # 帧1: 静止
        0.3,    # 帧2: 开始跳动
        0.7,    # 帧3: 跳动增强
        1.0,    # 帧4: 最大跳动
        0.8,    # 帧5: 开始收缩
        0.4,    # 帧6: 继续收缩
        0.1,    # 帧7: 接近静止
        0.0     # 帧8: 回到静止
    ]
    
    print("创建心跳动画...")
    
    for frame in range(frames):
        # 创建白色背景
        img = Image.new('RGB', (width, height), color='white')
        draw = ImageDraw.Draw(img)
        
        # 获取当前帧的心跳强度
        intensity = heartbeat_sequence[frame]
        
        # 绘制心跳图案
        draw_heartbeat_pattern(draw, center_x, center_y, intensity, frame)
        
        # 保存图片
        filename = f"sample_images/anim_emoji/heartbeat_frame_{frame+1:02d}.png"
        img.save(filename)
        print(f"创建: heartbeat_frame_{frame+1:02d}.png (强度: {intensity:.1f})")
    
    print(f"\n心跳动画创建完成！共{frames}帧")
    print("文件保存在: sample_images/anim_emoji/")

def draw_heartbeat_pattern(draw, center_x, center_y, intensity, frame):
    """绘制心跳图案"""
    
    # 基础大小
    base_size = 20
    
    # 根据强度计算当前大小
    current_size = int(base_size * (1 + intensity * 0.5))
    
    # 绘制心形
    draw_heart(draw, center_x, center_y, current_size, intensity)
    
    # 添加心跳线
    if intensity > 0.1:
        draw_heartbeat_line(draw, center_x, center_y, intensity, frame)

def draw_heart(draw, center_x, center_y, size, intensity):
    """绘制心形图案"""
    
    # 心形的基本参数
    heart_width = size
    heart_height = int(size * 0.8)
    
    # 心形的关键点
    left_x = center_x - heart_width // 2
    right_x = center_x + heart_width // 2
    top_y = center_y - heart_height // 2
    bottom_y = center_y + heart_height // 2
    
    # 绘制心形的两个圆形部分
    circle_radius = heart_width // 4
    
    # 左半圆
    left_circle_x = center_x - heart_width // 4
    draw.ellipse([left_circle_x - circle_radius, top_y, 
                 left_circle_x + circle_radius, top_y + circle_radius * 2], 
                outline='black', width=2)
    
    # 右半圆
    right_circle_x = center_x + heart_width // 4
    draw.ellipse([right_circle_x - circle_radius, top_y, 
                 right_circle_x + circle_radius, top_y + circle_radius * 2], 
                outline='black', width=2)
    
    # 绘制心形的底部三角形
    triangle_points = [
        (left_circle_x, top_y + circle_radius),
        (right_circle_x, top_y + circle_radius),
        (center_x, bottom_y)
    ]
    draw.polygon(triangle_points, outline='black', width=2)
    
    # 根据强度添加填充效果
    if intensity > 0.5:
        # 高强度时填充心形
        fill_points = [
            (left_circle_x - circle_radius//2, top_y + circle_radius//2),
            (right_circle_x + circle_radius//2, top_y + circle_radius//2),
            (center_x, bottom_y - 5)
        ]
        draw.polygon(fill_points, fill='black')

def draw_heartbeat_line(draw, center_x, center_y, intensity, frame):
    """绘制心跳线"""
    
    # 心跳线的长度和位置
    line_length = int(30 * intensity)
    line_y = center_y + 25
    
    # 绘制心跳线
    start_x = center_x - line_length // 2
    end_x = center_x + line_length // 2
    
    # 根据帧数添加不同的心跳效果
    if frame % 2 == 0:
        # 偶数帧：直线
        draw.line([start_x, line_y, end_x, line_y], fill='black', width=2)
    else:
        # 奇数帧：波浪线
        points = []
        for i in range(0, line_length, 4):
            x = start_x + i
            wave_y = line_y + int(3 * math.sin(i * 0.5) * intensity)
            points.append((x, wave_y))
        
        if len(points) > 1:
            draw.line(points, fill='black', width=2)

def create_blink_animation():
    """创建8帧眨眼动画"""
    
    print("\n创建眨眼动画...")
    
    # 眨眼动画的帧序列
    blink_sequence = [
        1.0,    # 帧1: 完全睁开
        1.0,    # 帧2: 保持睁开
        0.8,    # 帧3: 开始闭眼
        0.4,    # 帧4: 半闭
        0.0,    # 帧5: 完全闭上
        0.4,    # 帧6: 开始睁开
        0.8,    # 帧7: 半开
        1.0     # 帧8: 完全睁开
    ]
    
    width, height = 128, 64
    center_x, center_y = width // 2, height // 2
    
    for frame in range(8):
        # 创建白色背景
        img = Image.new('RGB', (width, height), color='white')
        draw = ImageDraw.Draw(img)
        
        # 获取当前帧的睁眼程度
        eye_openness = blink_sequence[frame]
        
        # 绘制眨眼表情
        draw_blink_face(draw, center_x, center_y, eye_openness)
        
        # 保存图片
        filename = f"sample_images/anim_emoji/blink_frame_{frame+1:02d}.png"
        img.save(filename)
        print(f"创建: blink_frame_{frame+1:02d}.png (睁眼度: {eye_openness:.1f})")
    
    print("眨眼动画创建完成！")

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

def main():
    """主函数"""
    print("创建动态图案示例")
    print("=" * 40)
    
    # 创建心跳动画
    create_heartbeat_animation()
    
    # 创建眨眼动画
    create_blink_animation()
    
    print("\n=== 使用说明 ===")
    print("1. 查看生成的动画图片: sample_images/anim_emoji/")
    print("2. 使用转换工具转换每一帧:")
    print("   python image_to_bitmap.py sample_images/anim_emoji/heartbeat_frame_01.png 120")
    print("3. 将生成的数组复制到C代码中")
    print("4. 在bitmap_emoji_display.cc中创建动画对象")

if __name__ == "__main__":
    main()
