
#pragma once

#include "game_window.h"
#include "../vulkan_base/game_device.h"
#include "../vulkan_base/game_buffer.h"
#include "../vulkan_base/game_descriptors.h"
#include "../vulkan_base_render/game_renderer.h"
#include "../vulkan_base_render/game_model.h"
#include "../vulkan_base_render/game_texture.h"
#include "../game_render_system/base_render_system.h"
#include "../game_render_system/point_light_system.h"
#include "../gameplay_logic/game_camera.h"
#include "../input_system/mouse_rotate_controller.h"
#include "../input_system/keyboard_input_controller.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <iostream>
#include <memory>
#include <string>
#include <array>
#include <cassert>
#include <chrono> // 高分辨率时间
#include <stdexcept>
