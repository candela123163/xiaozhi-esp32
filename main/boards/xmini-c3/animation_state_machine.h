#pragma once

#include <string>
#include <unordered_map>
#include <vector>


// 前向声明
namespace bitmap_emoji {
    class BitmapEmojiDisplay;
}

namespace animation_state {

// 系统状态枚举
enum class SystemState {
    UNKNOWN,    // 未知状态（初始状态）
    SYSTEM,     // 系统状态（启动、初始化等）
    IDLE,       // 空闲状态（待机）
    LISTENING,  // 监听状态（等待用户输入）
    SPEAKING    // 说话状态（播放语音）
};

// 状态转移结果
enum class TransitionResult {
    SUCCESS,           // 转移成功
    INVALID_TRANSITION, // 无效转移（不能转移到当前状态）
    INVALID_STATE,     // 无效状态
    CALLBACK_FAILED    // 回调函数执行失败
};


// 状态机类
class AnimationStateMachine {
public:
    // 构造函数
    AnimationStateMachine();
    
    // 析构函数
    ~AnimationStateMachine() = default;
    
    // 获取当前状态
    SystemState GetCurrentState() const { return current_state_; }
    
    // 获取当前状态名称
    std::string GetCurrentStateName() const;
    
    // 状态转移接口
    TransitionResult TransitionTo(SystemState target_state);
    
    // 设置动画显示模块引用
    void SetAnimationDisplay(bitmap_emoji::BitmapEmojiDisplay* display);
    
    // 检查是否可以转移到目标状态
    bool CanTransitionTo(SystemState target_state) const;
    
    // 获取所有可能的状态转移
    std::vector<SystemState> GetPossibleTransitions() const;
    
    // 重置状态机到初始状态
    void Reset();

private:
    // 当前状态
    SystemState current_state_;
    
    // 动画显示模块引用
    bitmap_emoji::BitmapEmojiDisplay* animation_display_;
    
    // 状态名称映射
    std::unordered_map<SystemState, std::string> state_names_;
    
    // 初始化状态名称映射
    void InitializeStateNames();
    
    // 执行状态转移
    TransitionResult ExecuteTransition(SystemState target_state);
    
    // 验证状态转移是否有效
    bool ValidateTransition(SystemState from_state, SystemState to_state) const;
    
    // 播放状态对应的动画
    void PlayStateAnimation(SystemState state);
};

} // namespace animation_state
