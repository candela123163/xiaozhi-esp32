# Xmini-C3 Bitmap Emoji Display 实现说明

## 概述

本项目为Xmini-C3开发板实现了一个基于ESP-IDF原生API的位图表情显示系统，替代了原有的anim_player依赖，使用`esp_timer` + `esp_lcd_panel_draw_bitmap()`的方案。

## 核心特性

### 1. 基于ESP-IDF原生API
- 使用`esp_lcd_panel_draw_bitmap()`进行位图显示
- 使用`esp_timer`实现动画定时控制
- 完全兼容SSD1306 OLED显示屏

### 2. 表情系统架构
```
BitmapEmojiDisplay (主显示类)
└── BitmapEmojiPlayer (动画播放器)
    ├── EmojiFrame (单帧数据结构)
    ├── EmojiAnimation (动画数据结构)
    └── esp_timer (定时器控制)
```

### 3. 支持的功能
- ✅ 单帧位图显示
- ✅ 多帧动画播放
- ✅ 循环/非循环播放
- ✅ 可配置帧率
- ✅ 表情映射系统
- ✅ 状态显示
- ✅ 通知显示

## 文件结构

```
main/boards/xmini-c3/
├── bitmap_emoji_display.h    # 头文件定义
├── bitmap_emoji_display.cc   # 实现文件
├── xmini_c3_board.cc         # 板卡集成
└── README_bitmap_emoji.md    # 说明文档
```

## 核心API

### BitmapEmojiPlayer类

```cpp
// 播放动画
void PlayAnimation(const EmojiAnimation& animation);

// 停止播放
void StopAnimation();

// 设置单帧位图
void SetBitmap(const uint8_t* bitmap_data, uint16_t width, uint16_t height);

// 清除屏幕
void ClearScreen();
```

### BitmapEmojiDisplay类

```cpp
// 设置表情
void SetEmotion(const char* emotion);

// 设置状态
void SetStatus(const char* status);

// 显示通知
void ShowNotification(const char* notification, int duration_ms = 3000);
```

## 位图数据格式

### 1位位图数据要求
- **格式**: 1位单色位图
- **存储**: 每8个像素占用1个字节
- **排列**: 按行存储，每行从左到右
- **示例**: 32x32像素需要128字节数据

### 位图数据示例
```cpp
// 32x32像素的开心表情
const uint8_t happy_face[] = {
    0x00, 0x00, 0x00, 0x00,  // 第1行 (32像素 = 4字节)
    0x00, 0x00, 0x00, 0x00,  // 第2行
    // ... 更多行数据
    0x00, 0x00, 0x00, 0x00   // 第32行
};
```

## 使用方法

### 1. 基本表情显示
```cpp
// 在Application中设置表情
auto& app = Application::GetInstance();
app.SetEmotion("happy");    // 显示开心表情
app.SetEmotion("sad");      // 显示悲伤表情
```

### 2. 状态显示
```cpp
app.SetStatus("listening"); // 显示监听状态
app.SetStatus("thinking");  // 显示思考状态
```

### 3. 通知显示
```cpp
app.ShowNotification("Hello World!", 3000); // 显示3秒通知
```

## 自定义表情

### 1. 添加新的位图数据
```cpp
// 在bitmap_emoji_display.cc中添加新的位图
const uint8_t custom_face[] = {
    // 你的位图数据...
};
```

### 2. 注册到表情映射
```cpp
void BitmapEmojiDisplay::SetupEmotionMappings() {
    EmojiAnimation custom_anim;
    custom_anim.frames = new EmojiFrame[1]{
        EmojiFrame(custom_face, 32, 32, 100)
    };
    custom_anim.frame_count = 1;
    custom_anim.repeat = true;
    custom_anim.fps = 10;
    emotion_map_["custom"] = custom_anim;
}
```

### 3. 使用自定义表情
```cpp
app.SetEmotion("custom");
```

## 动画配置

### EmojiAnimation结构
```cpp
struct EmojiAnimation {
    const EmojiFrame* frames;    // 帧数组
    uint8_t frame_count;         // 帧数量
    uint8_t current_frame;       // 当前帧索引
    bool is_playing;             // 是否正在播放
    bool repeat;                 // 是否循环播放
    int fps;                     // 播放帧率
};
```

### EmojiFrame结构
```cpp
struct EmojiFrame {
    const uint8_t* bitmap_data;  // 位图数据指针
    uint16_t width;              // 位图宽度
    uint16_t height;             // 位图高度
    uint32_t duration_ms;        // 显示持续时间(毫秒)
};
```

## 性能优化

### 1. 内存管理
- 位图数据使用`const uint8_t*`存储，节省RAM
- 动画帧数据在栈上分配，避免内存泄漏

### 2. 显示优化
- 使用`esp_lcd_panel_draw_bitmap()`直接绘制，无需中间缓冲
- 定时器精度为微秒级，确保流畅动画

### 3. 资源使用
- 任务栈大小: 4KB
- 定时器开销: 最小
- 内存占用: 仅位图数据大小

## 与EmojiWidget的对比

| 特性 | EmojiWidget | BitmapEmojiDisplay |
|------|-------------|-------------------|
| **依赖库** | anim_player (内部) | ESP-IDF原生API |
| **文件格式** | .aaf格式 | 直接位图数据 |
| **内存使用** | 较高 | 较低 |
| **可扩展性** | 受限于AAF | 完全自定义 |
| **维护性** | 依赖内部库 | 标准API |
| **兼容性** | 特定项目 | 通用ESP-IDF |

## 故障排除

### 1. 编译错误
- 确保包含正确的头文件
- 检查ESP-IDF版本兼容性

### 2. 显示问题
- 验证位图数据格式
- 检查SSD1306初始化
- 确认I2C通信正常

### 3. 动画问题
- 检查定时器创建
- 验证帧率设置
- 确认位图数据完整性

## 扩展建议

### 1. 多表情动画
- 创建多帧动画序列
- 实现表情过渡效果
- 添加动画插值

### 2. 交互功能
- 添加触摸响应
- 实现表情切换动画
- 支持用户自定义表情

### 3. 性能优化
- 实现位图压缩
- 添加缓存机制
- 优化内存使用

## 参考资源

- [ESP-IDF LCD组件文档](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd.html)
- [ESP-IDF定时器文档](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html)
- [SSD1306 OLED驱动](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/lcd/i2c_oled)
