#!/usr/bin/env python3
"""
预览功能测试脚本
演示图片转换和预览功能
"""

import os
import sys
from PIL import Image, ImageDraw
import numpy as np

def create_test_image():
    """创建一个测试图片"""
    img = Image.new('RGB', (128, 64), color='white')
    draw = ImageDraw.Draw(img)
    
    # 画一个简单的笑脸
    # 脸
    draw.ellipse([20, 10, 108, 54], outline='black', width=2)
    
    # 眼睛
    draw.ellipse([35, 20, 45, 30], fill='black')  # 左眼
    draw.ellipse([83, 20, 93, 30], fill='black')  # 右眼
    
    # 嘴巴 (微笑)
    draw.arc([40, 35, 88, 50], start=0, end=180, fill='black', width=2)
    
    # 保存测试图片
    img.save("test_emoji.png")
    print("创建测试图片: test_emoji.png")
    return "test_emoji.png"

def test_different_thresholds():
    """测试不同阈值的转换效果"""
    
    # 导入转换函数
    sys.path.append('.')
    from image_to_bitmap import image_to_bitmap_array
    
    image_path = create_test_image()
    
    # 测试不同的阈值
    thresholds = [80, 120, 160, 200]
    
    print("\n=== 测试不同阈值的转换效果 ===")
    
    for threshold in thresholds:
        print(f"\n阈值 {threshold}:")
        try:
            # 转换并生成预览
            array_str = image_to_bitmap_array(image_path, threshold, None, save_preview=True)
            print(f"  预览图片: test_emoji_preview_t{threshold}.png")
        except Exception as e:
            print(f"  转换失败: {e}")
    
    print("\n请查看生成的预览图片，选择效果最好的阈值！")

def compare_preview_with_original():
    """比较预览图片和原图"""
    
    print("\n=== 预览图片对比 ===")
    print("预览图片命名规则: 原文件名_preview_t阈值.png")
    print("例如: test_emoji_preview_t120.png")
    print("\n预览图片显示:")
    print("- 黑色像素: 原图中亮度低于阈值的像素")
    print("- 白色像素: 原图中亮度高于阈值的像素")
    print("\n建议:")
    print("- 如果预览图片太暗，降低阈值")
    print("- 如果预览图片太亮，提高阈值")
    print("- 选择细节最清晰的阈值")

def main():
    """主函数"""
    print("图片转位图预览功能测试")
    print("=" * 40)
    
    # 测试不同阈值
    test_different_thresholds()
    
    # 说明预览功能
    compare_preview_with_original()
    
    print("\n=== 使用建议 ===")
    print("1. 查看生成的预览图片")
    print("2. 选择效果最好的阈值")
    print("3. 使用该阈值进行最终转换")
    print("4. 将生成的数组复制到代码中")

if __name__ == "__main__":
    main()
