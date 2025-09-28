# 图片转位图工具使用说明

## 📋 工具介绍

`image_to_bitmap.py` 是一个Python脚本，用于将128x64像素的图片转换为SSD1306 OLED兼容的1位位图数组。

## 🎯 功能特性

- **多格式支持**: PNG, JPG, JPEG, BMP, GIF
- **智能转换**: RGB图片根据亮度阈值自动转换为黑白
- **SSD1306兼容**: 输出符合SSD1306垂直排列格式的位图数据
- **批量处理**: 支持批量转换整个目录的图片
- **测试图案**: 内置测试图案生成功能

## 📦 依赖安装

```bash
pip install Pillow numpy
```

## 🚀 使用方法

### 1. 单文件转换

```bash
# 基本用法 - 输出到控制台，生成预览图片
python image_to_bitmap.py happy.png

# 指定亮度阈值 (默认128)，生成预览图片
python image_to_bitmap.py happy.png 100

# 输出到文件，生成预览图片
python image_to_bitmap.py happy.png 128 happy_bitmap.txt

# 不生成预览图片
python image_to_bitmap.py happy.png 128 happy_bitmap.txt --no-preview
```

### 2. 批量转换

```bash
# 转换整个目录的图片，生成预览图片
python image_to_bitmap.py --batch ./images/ emoji_data.cc

# 指定亮度阈值，生成预览图片
python image_to_bitmap.py --batch ./images/ emoji_data.cc 100

# 不生成预览图片
python image_to_bitmap.py --batch ./images/ emoji_data.cc 100 --no-preview
```

### 3. 创建测试图案

```bash
# 生成测试图案
python image_to_bitmap.py --create-test
```

## 📐 图片要求

- **分辨率**: 必须是128x64像素
- **格式**: 支持RGB/RGBA图片
- **内容**: 可以是任何图像内容

## ⚙️ 参数说明

### 亮度阈值 (threshold)
- **范围**: 0-255
- **默认**: 128
- **作用**: 像素亮度低于此值视为黑色(0)，高于此值视为白色(1)

### 预览图片功能
- **默认**: 启用
- **文件名格式**: `原文件名_preview_t阈值.png`
- **作用**: 保存转换后的单色图片，便于快速查看转换效果
- **禁用**: 使用 `--no-preview` 参数

### 输出格式
生成的C语言数组格式：
```c
// image_name - 128x64像素，1位单色位图
const uint8_t image_name[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    // ... 更多数据
};
```

## 🎨 测试图案

运行 `--create-test` 会生成以下测试图案：

1. **test_border.png** - 边框图案
2. **test_stripes.png** - 条纹图案  
3. **test_checker.png** - 棋盘图案
4. **test_gradient.png** - 渐变图案

## 📝 使用示例

### 示例1: 转换单个表情图片

```bash
# 假设你有一个128x64的笑脸图片
python image_to_bitmap.py smile.png 120 smile_bitmap.txt
```

转换完成后会生成：
- `smile_bitmap.txt` - C语言数组文件
- `smile_preview_t120.png` - 转换后的单色预览图片

输出文件内容：
```c
// smile - 128x64像素，1位单色位图
const uint8_t smile[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // ... 位图数据
};
```

预览图片 `smile_preview_t120.png` 显示转换后的黑白效果，可以快速验证转换质量。

### 示例2: 批量转换表情包

```bash
# 准备图片目录
mkdir emoji_images
# 将128x64的表情图片放入目录

# 批量转换（生成预览图片）
python image_to_bitmap.py --batch emoji_images/ emoji_data.cc 100
```

批量转换会为每个图片生成对应的预览图片：
- `happy_preview_t100.png`
- `sad_preview_t100.png`
- `thinking_preview_t100.png`
- 等等...

### 示例3: 创建测试图案并转换

```bash
# 生成测试图案
python image_to_bitmap.py --create-test

# 转换测试图案
python image_to_bitmap.py test_border.png 128
```

## 🔧 集成到项目

### 1. 生成位图数据

```bash
# 转换你的表情图片
python image_to_bitmap.py happy_face.png 120
python image_to_bitmap.py sad_face.png 120
python image_to_bitmap.py thinking_face.png 120
```

### 2. 更新emoji_data.cc

将生成的数组复制到 `emoji_data.cc` 文件中：

```cpp
#include "emoji_data.h"

namespace bitmap_emoji {

// 从转换工具生成的数组
const uint8_t happy_face[] = {
    // ... 位图数据
};

const uint8_t sad_face[] = {
    // ... 位图数据
};

// ... 其他表情

} // namespace bitmap_emoji
```

### 3. 更新SetupEmotionMappings

在 `bitmap_emoji_display.cc` 中注册新表情：

```cpp
void BitmapEmojiDisplay::SetupEmotionMappings() {
    emotion_map_["happy"] = EmojiAnimation(happy_face, EMOJI_WIDTH, EMOJI_HEIGHT, 100, true, 10);
    emotion_map_["sad"] = EmojiAnimation(sad_face, EMOJI_WIDTH, EMOJI_HEIGHT, 100, true, 10);
    // ... 其他表情
}
```

## 🎯 最佳实践

### 1. 图片准备
- 使用图像编辑软件调整图片为128x64像素
- 确保图片对比度足够，便于阈值转换
- 建议使用PNG格式保存

### 2. 阈值选择
- **高对比度图片**: 使用默认阈值128
- **较暗图片**: 降低阈值到80-100
- **较亮图片**: 提高阈值到150-180

### 3. 测试验证
- 先用测试图案验证转换效果
- 在实际硬件上测试显示效果
- 根据需要调整阈值重新转换

## 🐛 常见问题

### Q: 图片尺寸不是128x64怎么办？
A: 使用图像编辑软件调整图片尺寸，或者修改脚本中的尺寸检查。

### Q: 转换后的图案太暗/太亮？
A: 调整亮度阈值参数，降低阈值使更多像素变为白色，提高阈值使更多像素变为黑色。

### Q: 如何创建动画帧？
A: 准备多张128x64的图片，分别转换后组合成动画数组。

### Q: 支持其他分辨率吗？
A: 当前脚本专门为128x64设计，如需其他分辨率需要修改脚本。

## 📊 技术细节

### SSD1306格式说明
- **位排列**: 垂直排列，每8个垂直像素组成1个字节
- **字节顺序**: 从左到右，从上到下
- **位顺序**: MSB在上，LSB在下

### 亮度计算公式
```python
brightness = 0.299 * R + 0.587 * G + 0.114 * B
```

### 数据大小
- **总像素**: 128 × 64 = 8192像素
- **字节数**: 8192 ÷ 8 = 1024字节
- **Flash占用**: 1KB per表情
