#include "animation_state_machine.h"
#include "bitmap_emoji_display.h"
#include "esp_log.h"
#include <vector>

static const char* TAG = "animation_state_machine";

namespace animation_state {

AnimationStateMachine::AnimationStateMachine()
    : current_state_(SystemState::UNKNOWN), animation_display_(nullptr) {
    InitializeStateNames();
    ESP_LOGI(TAG, "Animation state machine initialized, current state: %s", 
             GetCurrentStateName().c_str());
}

std::string AnimationStateMachine::GetCurrentStateName() const {
    auto it = state_names_.find(current_state_);
    return (it != state_names_.end()) ? it->second : "UNKNOWN";
}

TransitionResult AnimationStateMachine::TransitionTo(SystemState target_state) {
    ESP_LOGI(TAG, "Attempting transition from %s to %s", 
             GetCurrentStateName().c_str(),
             state_names_[target_state].c_str());
    
    // 检查是否转移到当前状态
    if (target_state == current_state_) {
        ESP_LOGW(TAG, "Cannot transition to current state: %s", 
                 GetCurrentStateName().c_str());
        return TransitionResult::INVALID_TRANSITION;
    }
    
    // 验证状态转移是否有效
    if (!ValidateTransition(current_state_, target_state)) {
        ESP_LOGE(TAG, "Invalid transition from %s to %s", 
                 GetCurrentStateName().c_str(),
                 state_names_[target_state].c_str());
        return TransitionResult::INVALID_TRANSITION;
    }
    
    // 执行状态转移
    return ExecuteTransition(target_state);
}

void AnimationStateMachine::SetAnimationDisplay(bitmap_emoji::BitmapEmojiDisplay* display) {
    animation_display_ = display;
    ESP_LOGI(TAG, "Animation display reference set: %p", display);
}


bool AnimationStateMachine::CanTransitionTo(SystemState target_state) const {
    return ValidateTransition(current_state_, target_state);
}

std::vector<SystemState> AnimationStateMachine::GetPossibleTransitions() const {
    std::vector<SystemState> possible_transitions;
    
    for (const auto& state_pair : state_names_) {
        SystemState state = state_pair.first;
        if (ValidateTransition(current_state_, state)) {
            possible_transitions.push_back(state);
        }
    }
    
    return possible_transitions;
}

void AnimationStateMachine::Reset() {
    ESP_LOGI(TAG, "Resetting state machine to SYSTEM state");
    TransitionTo(SystemState::SYSTEM);
}

void AnimationStateMachine::InitializeStateNames() {
    state_names_[SystemState::UNKNOWN] = "UNKNOWN";
    state_names_[SystemState::SYSTEM] = "SYSTEM";
    state_names_[SystemState::IDLE] = "IDLE";
    state_names_[SystemState::LISTENING] = "LISTENING";
    state_names_[SystemState::SPEAKING] = "SPEAKING";
}

TransitionResult AnimationStateMachine::ExecuteTransition(SystemState target_state) {
    // 更新当前状态
    SystemState previous_state = current_state_;
    current_state_ = target_state;
    
    ESP_LOGI(TAG, "State transition successful: %s -> %s", 
             state_names_[previous_state].c_str(),
             state_names_[current_state_].c_str());
    
    // 播放状态对应的动画
    PlayStateAnimation(current_state_);
    
    return TransitionResult::SUCCESS;
}

bool AnimationStateMachine::ValidateTransition(SystemState from_state, SystemState to_state) const {
    // 基础验证：不能转移到当前状态
    if (from_state == to_state) {
        return false;
    }
    
    // 状态转移规则定义
    switch (from_state) {
        case SystemState::UNKNOWN:
            // UNKNOWN状态可以转移到任何其他状态
            return (to_state != SystemState::UNKNOWN);
            
        case SystemState::SYSTEM:
        case SystemState::IDLE:
        case SystemState::LISTENING:
        case SystemState::SPEAKING:
            // 其他状态可以转移到任何状态，但不能转移到UNKNOWN
            return (to_state != SystemState::UNKNOWN);
            
        default:
            ESP_LOGE(TAG, "Unknown state: %d", static_cast<int>(from_state));
            return false;
    }
}

void AnimationStateMachine::PlayStateAnimation(SystemState state) {
    if (!animation_display_) {
        ESP_LOGW(TAG, "Animation display not set, skipping animation for state: %s", 
                 state_names_[state].c_str());
        return;
    }
    
    ESP_LOGI(TAG, "Playing animation for state: %s", state_names_[state].c_str());
    
    // 根据状态播放对应的动画
    switch (state) {
        case SystemState::UNKNOWN:
            // UNKNOWN状态不播放动画，或者播放默认动画
            ESP_LOGI(TAG, "UNKNOWN state - no animation to play");
            break;
            
        case SystemState::SYSTEM:
            // 播放系统启动动画
            animation_display_->PlayAnimation("circul");
            break;
            
        case SystemState::IDLE:
            // 播放空闲动画
            animation_display_->PlayAnimation("particle");
            break;
            
        case SystemState::LISTENING:
            // 播放监听动画
            animation_display_->PlayAnimation("line");
            break;
            
        case SystemState::SPEAKING:
            // 播放说话动画
            animation_display_->PlayAnimation("lotus");
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown state for animation: %d", static_cast<int>(state));
            break;
    }
}

} // namespace animation_state
