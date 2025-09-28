#pragma once

#include "display/display.h"
#include <memory>
#include <functional>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// 前向声明
namespace animation_state {
    class AnimationStateMachine;
}

namespace bitmap_emoji {

// 表情帧数据结构
struct EmojiFrame {
    const uint8_t* bitmap_data;  // 位图数据指针
    uint16_t width;              // 位图宽度
    uint16_t height;             // 位图高度
    uint32_t duration_ms;        // 显示持续时间(毫秒)
    
    EmojiFrame() : bitmap_data(nullptr), width(0), height(0), duration_ms(100) {}
    EmojiFrame(const uint8_t* data, uint16_t w, uint16_t h, uint32_t duration = 100)
        : bitmap_data(data), width(w), height(h), duration_ms(duration) {}
};

// 表情动画类
class EmojiAnimation {
public:
    // 构造函数
    EmojiAnimation() : frames_(nullptr), frame_count_(0), current_frame_(0), 
                      is_playing_(false), repeat_(true), fps_(10) {}
    
    // 构造函数 - 创建单帧动画
    EmojiAnimation(const uint8_t* bitmap_data, uint16_t width, uint16_t height, 
                   uint32_t duration_ms = 100, bool repeat = true, int fps = 10)
        : frame_count_(1), current_frame_(0), is_playing_(false), repeat_(repeat), fps_(fps) {
        frames_ = new EmojiFrame[1]{EmojiFrame(bitmap_data, width, height, duration_ms)};
    }
    
    // 构造函数 - 创建多帧动画
    EmojiAnimation(const EmojiFrame* frames, uint8_t count, bool repeat = true, int fps = 10)
        : frame_count_(count), current_frame_(0), is_playing_(false), repeat_(repeat), fps_(fps) {
        if (frames && count > 0) {
            frames_ = new EmojiFrame[count];
            for (uint8_t i = 0; i < count; i++) {
                frames_[i] = frames[i];
            }
        } else {
            frames_ = nullptr;
            frame_count_ = 0;
        }
    }
    
    // 析构函数
    ~EmojiAnimation() {
        if (frames_) {
            delete[] frames_;
            frames_ = nullptr;
        }
    }
    
    // 拷贝构造函数
    EmojiAnimation(const EmojiAnimation& other) 
        : frame_count_(other.frame_count_), current_frame_(other.current_frame_),
          is_playing_(other.is_playing_), repeat_(other.repeat_), fps_(other.fps_) {
        if (other.frames_ && frame_count_ > 0) {
            frames_ = new EmojiFrame[frame_count_];
            for (uint8_t i = 0; i < frame_count_; i++) {
                frames_[i] = other.frames_[i];
            }
        } else {
            frames_ = nullptr;
        }
    }
    
    // 移动构造函数
    EmojiAnimation(EmojiAnimation&& other) noexcept
        : frames_(other.frames_), frame_count_(other.frame_count_), 
          current_frame_(other.current_frame_), is_playing_(other.is_playing_),
          repeat_(other.repeat_), fps_(other.fps_) {
        other.frames_ = nullptr;
        other.frame_count_ = 0;
    }
    
    // 拷贝赋值运算符
    EmojiAnimation& operator=(const EmojiAnimation& other) {
        if (this != &other) {
            // 释放当前资源
            if (frames_) {
                delete[] frames_;
            }
            
            // 复制新资源
            frame_count_ = other.frame_count_;
            current_frame_ = other.current_frame_;
            is_playing_ = other.is_playing_;
            repeat_ = other.repeat_;
            fps_ = other.fps_;
            
            if (other.frames_ && frame_count_ > 0) {
                frames_ = new EmojiFrame[frame_count_];
                for (uint8_t i = 0; i < frame_count_; i++) {
                    frames_[i] = other.frames_[i];
                }
            } else {
                frames_ = nullptr;
            }
        }
        return *this;
    }
    
    // 移动赋值运算符
    EmojiAnimation& operator=(EmojiAnimation&& other) noexcept {
        if (this != &other) {
            // 释放当前资源
            if (frames_) {
                delete[] frames_;
            }
            
            // 移动资源
            frames_ = other.frames_;
            frame_count_ = other.frame_count_;
            current_frame_ = other.current_frame_;
            is_playing_ = other.is_playing_;
            repeat_ = other.repeat_;
            fps_ = other.fps_;
            
            // 清空源对象
            other.frames_ = nullptr;
            other.frame_count_ = 0;
        }
        return *this;
    }
    
    // 访问器方法
    const EmojiFrame* frames() const { return frames_; }
    uint8_t frame_count() const { return frame_count_; }
    uint8_t current_frame() const { return current_frame_; }
    bool is_playing() const { return is_playing_; }
    bool repeat() const { return repeat_; }
    int fps() const { return fps_; }
    
    // 修改器方法
    void set_current_frame(uint8_t frame) { current_frame_ = frame; }
    void set_is_playing(bool playing) { is_playing_ = playing; }
    
    // 检查是否有效
    bool is_valid() const { return frames_ != nullptr && frame_count_ > 0; }

private:
    EmojiFrame* frames_;      // 帧数组
    uint8_t frame_count_;     // 帧数量
    uint8_t current_frame_;   // 当前帧索引
    bool is_playing_;         // 是否正在播放
    bool repeat_;             // 是否循环播放
    int fps_;                 // 播放帧率
};

// 位图表情播放器类
class BitmapEmojiPlayer {
public:
    BitmapEmojiPlayer(esp_lcd_panel_handle_t panel, esp_lcd_panel_io_handle_t panel_io);
    ~BitmapEmojiPlayer();

    // 播放表情动画
    void PlayAnimation(const EmojiAnimation& animation);
    
    // 停止播放
    void StopAnimation();
    
    // 设置单帧位图
    void SetBitmap(const uint8_t* bitmap_data, uint16_t width, uint16_t height);
    
    // 清除屏幕
    void ClearScreen();

private:
    // 定时器回调函数
    static void TimerCallback(void* arg);
    
    // 更新帧逻辑
    void UpdateFrame();
    
    // 绘制当前帧
    void DrawCurrentFrame();
    
    // 切换到下一帧
    void NextFrame();

    esp_lcd_panel_handle_t panel_;
    esp_lcd_panel_io_handle_t panel_io_;
    esp_timer_handle_t timer_;
    
    EmojiAnimation current_animation_;
    bool is_initialized_;
    
    // 帧时长控制
    uint64_t frame_start_time_;  // 当前帧开始时间
    bool use_custom_duration_;   // 是否使用自定义帧时长
};

// 位图表情显示类
class BitmapEmojiDisplay : public Display {
public:
    BitmapEmojiDisplay(esp_lcd_panel_handle_t panel, esp_lcd_panel_io_handle_t panel_io);
    virtual ~BitmapEmojiDisplay();

    // Display接口实现
    virtual void SetEmotion(const char* emotion) override;
    virtual void SetStatus(const char* status) override;
    virtual void SetChatMessage(const char* role, const char* content) override;
    virtual void ShowNotification(const char* notification, int duration_ms = 3000) override;
    virtual void SetTheme(Theme* theme) override;
    virtual void SetPowerSaveMode(bool on) override;

    // 获取播放器实例
    BitmapEmojiPlayer* GetPlayer() { return player_.get(); }
    
    // 获取状态机实例
    animation_state::AnimationStateMachine* GetStateMachine() { return state_machine_.get(); }
    
    // 根据动画名称播放动画
    bool PlayAnimation(const char* animation_name);

private:
    virtual bool Lock(int timeout_ms = 0) override;
    virtual void Unlock() override;

    void InitializePlayer(esp_lcd_panel_handle_t panel, esp_lcd_panel_io_handle_t panel_io);
    
    // 动画映射表
    void SetupEmotionMappings();
    // 获取动画
    EmojiAnimation GetAnimation(const char* name);

    std::unique_ptr<BitmapEmojiPlayer> player_;
    
    // 状态机组合
    std::unique_ptr<animation_state::AnimationStateMachine> state_machine_;
    
    // 动画映射（统一管理所有动画）
    std::unordered_map<std::string, EmojiAnimation> animation_map_;
};

} // namespace bitmap_emoji
