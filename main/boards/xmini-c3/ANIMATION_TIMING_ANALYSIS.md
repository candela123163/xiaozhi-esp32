# EmojiFrame.duration_ms 和 EmojiAnimation.fps 的区别与改进

## 当前实现的问题

### 1. 时间控制机制冲突
- **EmojiAnimation.fps**: 控制定时器触发频率（如10fps = 每100ms触发一次）
- **EmojiFrame.duration_ms**: 设计用于控制每帧显示时长，但**实际未被使用**

### 2. 当前代码流程
```cpp
// 定时器每 1000ms/fps 触发一次
uint64_t interval_us = (1000000ULL / current_animation_.fps());

// 每次触发都立即切换帧
void TimerCallback(void* arg) {
    DrawCurrentFrame();  // 绘制当前帧
    NextFrame();         // 立即切换到下一帧
}
```

## 问题分析

1. **fps控制过于简单**: 所有帧都以相同速度切换，无法实现"慢动作"或"快动作"效果
2. **duration_ms被忽略**: 每帧的个性化显示时长无法实现
3. **缺乏灵活性**: 无法实现复杂的动画时序控制

## 改进方案

### 方案1: 使用duration_ms控制帧切换
```cpp
class BitmapEmojiPlayer {
private:
    uint64_t frame_start_time_ = 0;  // 当前帧开始时间
    
public:
    void UpdateFrame() {
        const EmojiFrame& current_frame = current_animation_.frames()[current_animation_.current_frame()];
        uint64_t current_time = esp_timer_get_time() / 1000; // 毫秒
        
        if (frame_start_time_ == 0) {
            frame_start_time_ = current_time;
            DrawCurrentFrame();
        } else {
            uint64_t elapsed = current_time - frame_start_time_;
            
            if (elapsed >= current_frame.duration_ms) {
                NextFrame();
                frame_start_time_ = current_time;
                DrawCurrentFrame();
            }
        }
    }
};
```

### 方案2: fps作为最大帧率限制
```cpp
void UpdateFrame() {
    const EmojiFrame& current_frame = current_animation_.frames()[current_animation_.current_frame()];
    uint64_t current_time = esp_timer_get_time() / 1000;
    
    if (frame_start_time_ == 0) {
        frame_start_time_ = current_time;
        DrawCurrentFrame();
    } else {
        uint64_t elapsed = current_time - frame_start_time_;
        
        // 使用duration_ms和fps的最小值
        uint64_t min_duration = std::min(current_frame.duration_ms, 
                                       1000 / current_animation_.fps());
        
        if (elapsed >= min_duration) {
            NextFrame();
            frame_start_time_ = current_time;
            DrawCurrentFrame();
        }
    }
}
```

## 使用场景对比

### 当前实现 (仅使用fps)
```cpp
// 所有帧都以相同速度播放
EmojiAnimation anim(frames, 8, true, 10); // 10fps，每帧100ms
```

### 改进后 (使用duration_ms)
```cpp
// 每帧可以有不同的显示时长
EmojiFrame frames[8] = {
    EmojiFrame(frame1, 128, 64, 200),  // 200ms
    EmojiFrame(frame2, 128, 64, 100),  // 100ms
    EmojiFrame(frame3, 128, 64, 300),  // 300ms
    // ...
};
EmojiAnimation anim(frames, 8, true, 30); // 最大30fps
```

## 建议

1. **保留fps作为最大帧率限制**，防止动画过快
2. **使用duration_ms控制实际帧切换**，实现灵活的时序控制
3. **添加帧间过渡效果**，如淡入淡出
4. **支持动画暂停/恢复**功能

这样可以让心跳动画实现更自然的效果：
- 静止帧显示较长时间
- 跳动帧显示较短时间
- 最大跳动帧显示最长时间
