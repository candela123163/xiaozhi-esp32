# 快速开始指南

## 🚀 5分钟快速上手

### 1. 安装依赖
```bash
pip install Pillow numpy
```

### 2. 创建示例图片
```bash
python example_usage.py
```
这会创建一些128x64的示例表情图片。

### 3. 转换图片为位图数组
```bash
# 转换开心表情（会生成预览图片）
python image_to_bitmap.py sample_images/simple_smile.png 120

# 转换悲伤表情（会生成预览图片）
python image_to_bitmap.py sample_images/simple_sad.png 120

# 转换思考表情（会生成预览图片）
python image_to_bitmap.py sample_images/simple_thinking.png 120

# 不生成预览图片
python image_to_bitmap.py sample_images/simple_smile.png 120 --no-preview
```

转换完成后会生成：
- C语言数组（输出到控制台或文件）
- 预览图片（如：`simple_smile_preview_t120.png`）

### 4. 批量转换（可选）
```bash
# 批量转换（会为每个图片生成预览）
python image_to_bitmap.py --batch sample_images/ emoji_data.cc 120

# 批量转换（不生成预览）
python image_to_bitmap.py --batch sample_images/ emoji_data.cc 120 --no-preview
```

### 5. 集成到项目
将生成的数组复制到 `emoji_data.cc` 文件中，替换现有的空数组。

## 📋 完整工作流程

### 步骤1: 准备图片
- 使用任何图像编辑软件创建128x64像素的图片
- 保存为PNG格式
- 确保图片对比度足够

### 步骤2: 转换图片
```bash
python image_to_bitmap.py your_image.png [threshold]
```

### 步骤3: 复制数组
将生成的C语言数组复制到 `emoji_data.cc` 中。

### 步骤4: 注册表情
在 `bitmap_emoji_display.cc` 的 `SetupEmotionMappings()` 中添加：
```cpp
emotion_map_["your_emotion"] = EmojiAnimation(your_array, EMOJI_WIDTH, EMOJI_HEIGHT, 100, true, 10);
```

### 步骤5: 测试
```cpp
app.SetEmotion("your_emotion");
```

## 🎯 常用命令

```bash
# 查看帮助
python image_to_bitmap.py

# 创建测试图案
python image_to_bitmap.py --create-test

# 单文件转换（生成预览）
python image_to_bitmap.py image.png 128 output.txt

# 单文件转换（不生成预览）
python image_to_bitmap.py image.png 128 output.txt --no-preview

# 批量转换（生成预览）
python image_to_bitmap.py --batch ./images/ emoji_data.cc 128

# 批量转换（不生成预览）
python image_to_bitmap.py --batch ./images/ emoji_data.cc 128 --no-preview
```

## ⚡ 快速测试

1. 运行示例脚本创建测试图片
2. 转换测试图片
3. 复制数组到代码中
4. 编译并烧录到硬件
5. 观察OLED显示效果

## 🔧 故障排除

- **图片尺寸错误**: 确保图片是128x64像素
- **转换效果不好**: 调整亮度阈值参数
- **编译错误**: 检查数组语法是否正确
- **显示异常**: 验证位图数据格式
