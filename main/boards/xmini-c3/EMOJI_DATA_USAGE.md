# Emoji数据文件使用说明

## 📁 文件结构

```
main/boards/xmini-c3/
├── emoji_data.h          # 表情数据头文件
├── emoji_data.cc         # 表情数据实现文件
├── bitmap_emoji_display.h # 显示系统头文件
├── bitmap_emoji_display.cc # 显示系统实现文件
└── EMOJI_DATA_USAGE.md   # 使用说明文档
```

## 🎯 设计目标

1. **模块化设计**: 将表情数据与显示逻辑分离
2. **易于扩展**: 方便添加新的表情和动画
3. **内存优化**: 使用常量数据，节省RAM
4. **类型安全**: 使用常量定义，避免魔法数字

## 📊 数据结构

### 表情数据常量
```cpp
// 位图数据大小常量
constexpr uint16_t EMOJI_WIDTH = 32;      // 表情宽度
constexpr uint16_t EMOJI_HEIGHT = 32;     // 表情高度
constexpr size_t EMOJI_DATA_SIZE = 128;   // 数据大小(字节)
```

### 单帧表情
```cpp
// 基本表情
extern const uint8_t happy_face[];     // 开心表情
extern const uint8_t sad_face[];       // 悲伤表情
extern const uint8_t thinking_face[];  // 思考表情
```

### 多帧动画
```cpp
// 眨眼动画 - 5帧
extern const uint8_t blink_frame1[];  // 睁眼
extern const uint8_t blink_frame2[];  // 半闭眼
extern const uint8_t blink_frame3[];  // 闭眼
extern const uint8_t blink_frame4[];  // 半闭眼
extern const uint8_t blink_frame5[];  // 睁眼
```

## 🎨 5帧眨眼动画示例

### 动画序列
```
帧1: 睁眼状态 (200ms) → 帧2: 半闭眼 (100ms) → 帧3: 闭眼 (100ms) → 帧4: 半闭眼 (100ms) → 帧5: 睁眼 (200ms)
```

### 实现代码
```cpp
// 在SetupEmotionMappings()中
EmojiFrame blink_frames[5] = {
    EmojiFrame(blink_frame1, EMOJI_WIDTH, EMOJI_HEIGHT, 200),  // 睁眼
    EmojiFrame(blink_frame2, EMOJI_WIDTH, EMOJI_HEIGHT, 100),  // 半闭眼
    EmojiFrame(blink_frame3, EMOJI_WIDTH, EMOJI_HEIGHT, 100),  // 闭眼
    EmojiFrame(blink_frame4, EMOJI_WIDTH, EMOJI_HEIGHT, 100),  // 半闭眼
    EmojiFrame(blink_frame5, EMOJI_WIDTH, EMOJI_HEIGHT, 200)   // 睁眼
};
emotion_map_["blink"] = EmojiAnimation(blink_frames, 5, true, 15);
```

### 使用方法
```cpp
// 播放眨眼动画
auto& app = Application::GetInstance();
app.SetEmotion("blink");
```

## 🔧 添加新表情

### 1. 添加单帧表情

#### 在emoji_data.h中添加声明
```cpp
// 新表情声明
extern const uint8_t angry_face[];
```

#### 在emoji_data.cc中添加数据
```cpp
// 愤怒表情 - 32x32像素，1位单色
const uint8_t angry_face[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // ... 128字节的位图数据
};
```

#### 在SetupEmotionMappings()中注册
```cpp
emotion_map_["angry"] = EmojiAnimation(angry_face, EMOJI_WIDTH, EMOJI_HEIGHT, 100, true, 10);
```

### 2. 添加多帧动画

#### 在emoji_data.h中添加声明
```cpp
// 心跳动画 - 5帧
extern const uint8_t heartbeat_frame1[];
extern const uint8_t heartbeat_frame2[];
extern const uint8_t heartbeat_frame3[];
extern const uint8_t heartbeat_frame4[];
extern const uint8_t heartbeat_frame5[];
```

#### 在emoji_data.cc中添加数据
```cpp
// 心跳动画帧数据
const uint8_t heartbeat_frame1[] = {
    // 小心跳数据...
};

const uint8_t heartbeat_frame2[] = {
    // 中心跳数据...
};

// ... 更多帧
```

#### 在SetupEmotionMappings()中注册
```cpp
EmojiFrame heartbeat_frames[5] = {
    EmojiFrame(heartbeat_frame1, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
    EmojiFrame(heartbeat_frame2, EMOJI_WIDTH, EMOJI_HEIGHT, 100),
    EmojiFrame(heartbeat_frame3, EMOJI_WIDTH, EMOJI_HEIGHT, 100),
    EmojiFrame(heartbeat_frame4, EMOJI_WIDTH, EMOJI_HEIGHT, 100),
    EmojiFrame(heartbeat_frame5, EMOJI_WIDTH, EMOJI_HEIGHT, 150)
};
emotion_map_["heartbeat"] = EmojiAnimation(heartbeat_frames, 5, true, 20);
```

## 📐 位图数据格式

### SSD1306格式要求
- **分辨率**: 32x32像素
- **颜色深度**: 1位单色 (黑白)
- **数据大小**: 128字节 (32×32÷8)
- **排列方式**: 垂直排列，每8个垂直像素占用1个字节
- **位顺序**: MSB优先

### 数据布局
```
字节0: [像素(0,0)][像素(0,1)][像素(0,2)][像素(0,3)][像素(0,4)][像素(0,5)][像素(0,6)][像素(0,7)]
字节1: [像素(1,0)][像素(1,1)][像素(1,2)][像素(1,3)][像素(1,4)][像素(1,5)][像素(1,6)][像素(1,7)]
...
字节15: [像素(15,0)][像素(15,1)][像素(15,2)][像素(15,3)][像素(15,4)][像素(15,5)][像素(15,6)][像素(15,7)]
```

## 🛠️ 位图转换工具

### 使用在线工具
1. **Image2cpp**: https://lvgl.io/tools/imageconverter
2. **LCD Assistant**: 支持多种格式转换
3. **GIMP**: 图像编辑软件，可导出为C数组

### 转换步骤
1. 准备32x32像素的PNG图像
2. 转换为1位单色
3. 使用工具生成C数组
4. 确保数据格式为SSD1306垂直排列

## 🎮 使用示例

### 基本表情显示
```cpp
// 显示开心表情
app.SetEmotion("happy");

// 显示悲伤表情
app.SetEmotion("sad");

// 显示思考表情
app.SetEmotion("thinking");
```

### 动画表情显示
```cpp
// 播放眨眼动画
app.SetEmotion("blink");

// 播放心跳动画
app.SetEmotion("heartbeat");
```

### 自定义动画
```cpp
// 创建自定义动画
EmojiFrame custom_frames[3] = {
    EmojiFrame(frame1_data, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
    EmojiFrame(frame2_data, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
    EmojiFrame(frame3_data, EMOJI_WIDTH, EMOJI_HEIGHT, 200)
};
EmojiAnimation custom_anim(custom_frames, 3, true, 10);

// 播放自定义动画
player->PlayAnimation(custom_anim);
```

## 🔍 调试技巧

### 1. 验证位图数据
```cpp
// 检查数据大小
static_assert(sizeof(happy_face) == EMOJI_DATA_SIZE, "Happy face data size mismatch");

// 检查数据完整性
for (size_t i = 0; i < EMOJI_DATA_SIZE; i++) {
    printf("0x%02X, ", happy_face[i]);
    if ((i + 1) % 16 == 0) printf("\n");
}
```

### 2. 测试动画播放
```cpp
// 测试单帧
EmojiAnimation test_anim(happy_face, EMOJI_WIDTH, EMOJI_HEIGHT, 1000, false, 1);
player->PlayAnimation(test_anim);

// 测试多帧
EmojiFrame test_frames[2] = {
    EmojiFrame(happy_face, EMOJI_WIDTH, EMOJI_HEIGHT, 500),
    EmojiFrame(sad_face, EMOJI_WIDTH, EMOJI_HEIGHT, 500)
};
EmojiAnimation test_anim2(test_frames, 2, true, 2);
player->PlayAnimation(test_anim2);
```

## 📈 性能优化

### 1. 内存使用
- **Flash存储**: 所有位图数据存储在Flash中
- **RAM使用**: 仅动画播放时占用少量RAM
- **数据共享**: 相同帧可重复使用

### 2. 播放优化
- **帧率控制**: 根据动画类型调整帧率
- **循环优化**: 避免不必要的内存拷贝
- **定时器精度**: 使用esp_timer确保精确时序

## 🎯 最佳实践

### 1. 命名规范
- 单帧表情: `{emotion}_face`
- 多帧动画: `{animation}_frame{N}`
- 常量定义: 使用`constexpr`

### 2. 数据组织
- 按功能分组存储
- 使用命名空间避免冲突
- 提供清晰的注释

### 3. 错误处理
- 验证数据大小
- 检查指针有效性
- 提供默认表情

## 🔄 版本更新

### v1.0 - 基础版本
- 单帧表情支持
- 基本动画功能
- 模块化设计

### v1.1 - 动画增强
- 5帧眨眼动画示例
- 多帧动画支持
- 性能优化

### 未来计划
- 更多动画类型
- 动态表情生成
- 用户自定义表情
