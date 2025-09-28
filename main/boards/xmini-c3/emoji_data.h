#pragma once

#include <stdint.h>

namespace bitmap_emoji {

// 表情位图数据定义
// 所有位图都是32x32像素，1位单色格式
// 数据按SSD1306垂直排列格式存储

extern const uint8_t lotus_0[];
extern const uint8_t lotus_1[];
extern const uint8_t lotus_2[];
extern const uint8_t lotus_3[];
extern const uint8_t lotus_4[];
extern const uint8_t lotus_5[];
extern const uint8_t lotus_6[];
extern const uint8_t lotus_7[];
extern const uint8_t lotus_8[];
extern const uint8_t lotus_9[];
extern const uint8_t lotus_10[];
extern const uint8_t lotus_11[];
extern const uint8_t lotus_12[];
extern const uint8_t lotus_13[];
extern const uint8_t lotus_14[];
extern const uint8_t lotus_15[];
extern const uint8_t lotus_16[];
extern const uint8_t lotus_17[];
extern const uint8_t lotus_18[];
extern const uint8_t lotus_19[];
extern const uint8_t lotus_20[];
extern const uint8_t lotus_21[];
extern const uint8_t lotus_22[];
extern const uint8_t lotus_23[];
extern const uint8_t lotus_24[];
extern const uint8_t lotus_25[];
extern const uint8_t lotus_26[];

extern const uint8_t particle_0[];
extern const uint8_t particle_1[];
extern const uint8_t particle_2[];
extern const uint8_t particle_3[];
extern const uint8_t particle_4[];
extern const uint8_t particle_5[];
extern const uint8_t particle_6[];
extern const uint8_t particle_7[];
extern const uint8_t particle_8[];
extern const uint8_t particle_9[];
extern const uint8_t particle_10[];
extern const uint8_t particle_11[];
extern const uint8_t particle_12[];
extern const uint8_t particle_13[];
extern const uint8_t particle_14[];
extern const uint8_t particle_15[];
extern const uint8_t particle_16[];
extern const uint8_t particle_17[];
extern const uint8_t particle_18[];
extern const uint8_t particle_19[];
extern const uint8_t particle_20[];
extern const uint8_t particle_21[];
extern const uint8_t particle_22[];
extern const uint8_t particle_23[];
extern const uint8_t particle_24[];
extern const uint8_t particle_25[];

extern const uint8_t circul_0[];
extern const uint8_t circul_1[];
extern const uint8_t circul_2[];
extern const uint8_t circul_3[];
extern const uint8_t circul_4[];
extern const uint8_t circul_5[];
extern const uint8_t circul_6[];
extern const uint8_t circul_7[];
extern const uint8_t circul_8[];
extern const uint8_t circul_9[];
extern const uint8_t circul_10[];
extern const uint8_t circul_11[];
extern const uint8_t circul_12[];
extern const uint8_t circul_13[];
extern const uint8_t circul_14[];
extern const uint8_t circul_15[];
extern const uint8_t circul_16[];
extern const uint8_t circul_17[];
extern const uint8_t circul_18[];
extern const uint8_t circul_19[];
extern const uint8_t circul_20[];
extern const uint8_t circul_21[];
extern const uint8_t circul_22[];

extern const uint8_t line_0[];
extern const uint8_t line_1[];
extern const uint8_t line_2[];
extern const uint8_t line_3[];
extern const uint8_t line_4[];
extern const uint8_t line_5[];
extern const uint8_t line_6[];
extern const uint8_t line_7[];
extern const uint8_t line_8[];
extern const uint8_t line_9[];
extern const uint8_t line_10[];
extern const uint8_t line_11[];
extern const uint8_t line_12[];
extern const uint8_t line_13[];
extern const uint8_t line_14[];
extern const uint8_t line_15[];
extern const uint8_t line_16[];
extern const uint8_t line_17[];
extern const uint8_t line_18[];
extern const uint8_t line_19[];
extern const uint8_t line_20[];
extern const uint8_t line_21[];
extern const uint8_t line_22[];
extern const uint8_t line_23[];
extern const uint8_t line_24[];
extern const uint8_t line_25[];


// 位图数据大小常量 - 使用屏幕分辨率
constexpr uint16_t EMOJI_WIDTH = 128;
constexpr uint16_t EMOJI_HEIGHT = 64;
constexpr uint32_t EMOJI_DATA_SIZE = (EMOJI_WIDTH * EMOJI_HEIGHT) / 8;  // 1024字节

} // namespace bitmap_emoji
