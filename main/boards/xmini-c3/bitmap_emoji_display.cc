#include "bitmap_emoji_display.h"
#include "animation_state_machine.h"
#include "emoji_data.h"
#include <esp_log.h>
#include <cstring>
#include <unordered_map>
#include "assets/lang_config.h"

static const char *TAG = "bitmap_emoji";

namespace bitmap_emoji {

// BitmapEmojiPlayer实现
BitmapEmojiPlayer::BitmapEmojiPlayer(esp_lcd_panel_handle_t panel, esp_lcd_panel_io_handle_t panel_io)
    : panel_(panel), panel_io_(panel_io), timer_(nullptr), is_initialized_(false),
      frame_start_time_(0), use_custom_duration_(false) {
    
    ESP_LOGI(TAG, "Create BitmapEmojiPlayer, panel: %p, panel_io: %p", panel, panel_io);
    
    // 创建定时器
    esp_timer_create_args_t timer_args = {
        .callback = TimerCallback,
        .arg = static_cast<void*>(this),
        .name = "emoji_timer"
        
    };
    
    if (esp_timer_create(&timer_args, &timer_) == ESP_OK) {
        is_initialized_ = true;
        ESP_LOGI(TAG, "BitmapEmojiPlayer initialized successfully");
    } else {
        ESP_LOGE(TAG, "Failed to create emoji timer");
    }
}

BitmapEmojiPlayer::~BitmapEmojiPlayer() {
    if (timer_) {
        esp_timer_stop(timer_);
        esp_timer_delete(timer_);
        timer_ = nullptr;
    }
}

void BitmapEmojiPlayer::PlayAnimation(const EmojiAnimation& animation) {
    if (!is_initialized_ || !timer_) {
        ESP_LOGE(TAG, "Player not initialized");
        return;
    }
    
    // 停止当前动画
    esp_timer_stop(timer_);
    
    // 设置新动画
    current_animation_ = animation;
    current_animation_.set_current_frame(0);
    current_animation_.set_is_playing(true);
    
    // 检测是否使用自定义帧时长
    use_custom_duration_ = false;
    if (current_animation_.is_valid()) {
        // 检查是否有帧设置了非默认的duration_ms
        for (uint8_t i = 0; i < current_animation_.frame_count(); i++) {
            if (current_animation_.frames()[i].duration_ms != 100) { // 100ms是默认值
                use_custom_duration_ = true;
                break;
            }
        }
    }
    
    // 重置帧时间
    frame_start_time_ = 0;
    
    // 计算定时器间隔 (微秒)
    uint64_t interval_us = (1000000ULL / current_animation_.fps());
    
    // 启动定时器
    if (esp_timer_start_periodic(timer_, interval_us) == ESP_OK) {
        ESP_LOGI(TAG, "Animation started: %d frames, %d fps, custom_duration: %s", 
                current_animation_.frame_count(), current_animation_.fps(),
                use_custom_duration_ ? "yes" : "no");
    } else {
        ESP_LOGE(TAG, "Failed to start animation timer");
    }
}

void BitmapEmojiPlayer::StopAnimation() {
    if (timer_) {
        esp_timer_stop(timer_);
        current_animation_.set_is_playing(false);
        ESP_LOGI(TAG, "Animation stopped");
    }
}

void BitmapEmojiPlayer::SetBitmap(const uint8_t* bitmap_data, uint16_t width, uint16_t height) {
    if (!is_initialized_ || !bitmap_data) {
        return;
    }
    
    // 停止动画
    StopAnimation();
    
    // 直接绘制位图
    esp_lcd_panel_draw_bitmap(panel_, 0, 0, width, height, bitmap_data);
}

void BitmapEmojiPlayer::ClearScreen() {
    if (!is_initialized_) {
        return;
    }
    
    // 创建全黑位图数据
    static uint8_t black_screen[128 * 64 / 8] = {0};
    esp_lcd_panel_draw_bitmap(panel_, 0, 0, 128, 64, black_screen);
}

void BitmapEmojiPlayer::TimerCallback(void* arg) {
    auto* player = static_cast<BitmapEmojiPlayer*>(arg);
    if (player && player->current_animation_.is_playing()) {
        player->UpdateFrame();
    }
}

void BitmapEmojiPlayer::UpdateFrame() {
    if (!current_animation_.is_valid()) {
        return;
    }
    
    const EmojiFrame& current_frame = current_animation_.frames()[current_animation_.current_frame()];
    uint64_t current_time = esp_timer_get_time() / 1000; // 转换为毫秒
    
    if (frame_start_time_ == 0) {
        // 第一帧，记录开始时间并绘制
        frame_start_time_ = current_time;
        DrawCurrentFrame();
    } else {
        uint64_t elapsed = current_time - frame_start_time_;
        
        if (use_custom_duration_) {
            // 使用自定义帧时长
            if (elapsed >= current_frame.duration_ms) {
                NextFrame();
                frame_start_time_ = current_time;
                DrawCurrentFrame();
            }
        } else {
            // 使用fps控制，每帧显示时间 = 1000ms / fps
            uint64_t frame_duration = 1000 / current_animation_.fps();
            if (elapsed >= frame_duration) {
                NextFrame();
                frame_start_time_ = current_time;
                DrawCurrentFrame();
            }
        }
    }
}

void BitmapEmojiPlayer::DrawCurrentFrame() {
    if (!current_animation_.is_valid()) {
        return;
    }
    
    const EmojiFrame& frame = current_animation_.frames()[current_animation_.current_frame()];
    if (frame.bitmap_data) {
        // 计算居中位置
        int x_offset = (128 - frame.width) / 2;
        int y_offset = (64 - frame.height) / 2;
        
        esp_lcd_panel_draw_bitmap(panel_, x_offset, y_offset, 
                                 x_offset + frame.width, y_offset + frame.height, 
                                 frame.bitmap_data);
    }
}

void BitmapEmojiPlayer::NextFrame() {
    if (current_animation_.frame_count() == 0) {
        return;
    }
    
    uint8_t next_frame = current_animation_.current_frame() + 1;
    current_animation_.set_current_frame(next_frame);
    
    // 检查是否需要循环
    if (next_frame >= current_animation_.frame_count()) {
        if (current_animation_.repeat()) {
            current_animation_.set_current_frame(0);
        } else {
            current_animation_.set_is_playing(false);
            esp_timer_stop(timer_);
        }
    }
}

// BitmapEmojiDisplay实现
BitmapEmojiDisplay::BitmapEmojiDisplay(esp_lcd_panel_handle_t panel, esp_lcd_panel_io_handle_t panel_io) {
    InitializePlayer(panel, panel_io);
    SetupEmotionMappings();
    
    // 初始化状态机
    state_machine_ = std::make_unique<animation_state::AnimationStateMachine>();
    state_machine_->SetAnimationDisplay(this);
    state_machine_->TransitionTo(animation_state::SystemState::SYSTEM);

    
    
    ESP_LOGI(TAG, "BitmapEmojiDisplay initialized with state machine");
}

BitmapEmojiDisplay::~BitmapEmojiDisplay() {
    // 智能指针会自动清理
}

void BitmapEmojiDisplay::SetEmotion(const char* emotion) {
    ESP_LOGI(TAG, "API Set emotion: %s, not supported for now!", emotion);
}

void BitmapEmojiDisplay::SetStatus(const char* status) {
    ESP_LOGI(TAG, "API Set status: %s.", status);

   if (status == Lang::Strings::LISTENING) {
    state_machine_->TransitionTo(animation_state::SystemState::LISTENING);
   } else if (status == Lang::Strings::STANDBY) {
    state_machine_->TransitionTo(animation_state::SystemState::IDLE);
   } else if (status == Lang::Strings::SPEAKING) {
    state_machine_->TransitionTo(animation_state::SystemState::SPEAKING);
   }
   else if (status == Lang::Strings::CONNECTING) {
    // do nothing
   }
   else {
    state_machine_->TransitionTo(animation_state::SystemState::SYSTEM);
   }
}

void BitmapEmojiDisplay::SetChatMessage(const char* role, const char* content) {
    ESP_LOGI(TAG, "API Set chat message: %s, %s, not supported for now!", role, content);
}

void BitmapEmojiDisplay::ShowNotification(const char* notification, int duration_ms) {
    ESP_LOGI(TAG, "API Show notification: %s, %d, not supported for now!", notification, duration_ms);
}

void BitmapEmojiDisplay::SetTheme(Theme* theme) {
    // SSD1306是单色显示，主题功能有限
    ESP_LOGI(TAG, "Theme set to: %s (limited support for monochrome display)", theme->name());
}

void BitmapEmojiDisplay::SetPowerSaveMode(bool on) {
    if (player_) {
        if (on) {
            player_->ClearScreen();
        }
        ESP_LOGI(TAG, "Power save mode: %s", on ? "ON" : "OFF");
    }
}

bool BitmapEmojiDisplay::Lock(int timeout_ms) {
    // 简单的互斥锁实现
    return true;
}

void BitmapEmojiDisplay::Unlock() {
    // 简单的互斥锁实现
}

void BitmapEmojiDisplay::InitializePlayer(esp_lcd_panel_handle_t panel, esp_lcd_panel_io_handle_t panel_io) {
    player_ = std::make_unique<BitmapEmojiPlayer>(panel, panel_io);
}

void BitmapEmojiDisplay::SetupEmotionMappings() {
    
    // 莲花动画
    EmojiFrame lotus_frames[27] = {
        EmojiFrame(lotus_0, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_1, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_2, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_3, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_4, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_5, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_6, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_7, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_8, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_9, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_10, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_11, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_12, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_13, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_14, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_15, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_16, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_17, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_18, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_19, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_20, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_21, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_22, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_23, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_24, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_25, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(lotus_26, EMOJI_WIDTH, EMOJI_HEIGHT, 200),   
    };
    animation_map_["lotus"] = EmojiAnimation(lotus_frames, 27, true, 30);
    
    // 粒子动画
    EmojiFrame particle_frames[26] = {
        EmojiFrame(particle_0, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_1, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_2, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_3, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_4, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_5, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_6, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_7, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_8, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_9, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_10, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_11, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_12, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_13, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_14, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_15, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_16, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_17, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_18, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_19, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_20, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_21, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_22, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_23, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_24, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(particle_25, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
    };
    animation_map_["particle"] = EmojiAnimation(particle_frames, 26, true, 30);
    
    // 添加更多动画...
    EmojiFrame circul_frames[23] = {
        EmojiFrame(circul_0, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_1, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_2, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_3, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_4, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_5, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_6, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_7, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_8, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_9, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_10, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_11, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_12, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_13, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_14, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_15, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_16, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_17, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_18, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_19, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_20, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_21, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
        EmojiFrame(circul_22, EMOJI_WIDTH, EMOJI_HEIGHT, 150),
    };
    animation_map_["circul"] = EmojiAnimation(circul_frames, 23, true, 30);

    EmojiFrame line_frames[26] = {
        EmojiFrame(line_0, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_1, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_2, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_3, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_4, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_5, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_6, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_7, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_8, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_9, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_10, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_11, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_12, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_13, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_14, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_15, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_16, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_17, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_18, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_19, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_20, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_21, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_22, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_23, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_24, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
        EmojiFrame(line_25, EMOJI_WIDTH, EMOJI_HEIGHT, 200),
    };
    animation_map_["line"] = EmojiAnimation(line_frames, 26, true, 30);
}

EmojiAnimation BitmapEmojiDisplay::GetAnimation(const char* name) {
    auto it = animation_map_.find(name);
    if (it != animation_map_.end()) {
        return it->second;
    }
    return EmojiAnimation();  // 返回空动画
}

bool BitmapEmojiDisplay::PlayAnimation(const char* animation_name) {
    ESP_LOGI(TAG, "Playing animation: %s", animation_name);
    
    if (!player_) {
        ESP_LOGE(TAG, "Player not initialized");
        return false;
    }
    
    if (!animation_name) {
        ESP_LOGE(TAG, "Animation name is null");
        return false;
    }
    
    // 在animation_map_中查找动画
    EmojiAnimation animation = GetAnimation(animation_name);
    if (!animation.is_valid()) {
        ESP_LOGW(TAG, "Animation not found: %s", animation_name);
        return false;
    }
    
    // 播放动画
    player_->PlayAnimation(animation);
    ESP_LOGI(TAG, "Successfully started animation: %s", animation_name);
    return true;
}

} // namespace bitmap_emoji
