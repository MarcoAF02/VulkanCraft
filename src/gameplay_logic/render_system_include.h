
#pragma once

#include "game_window.h"
#include "game_camera.h"
#include "../vulkan_base/game_device.h"
#include "../vulkan_base/game_buffer.h"
#include "../vulkan_base/game_descriptors.h"
#include "../vulkan_base_render/game_renderer.h"
#include "../vulkan_base_render/game_model.h"
#include "../vulkan_base_render/game_texture.h"
#include "../game_render_system/base_render_system.h"
#include "../game_render_system/point_light_system.h"
#include "../gameplay_logic/game_camera.h"
#include "../gameplay_logic/base_game_object.h"
#include "../input_system/mouse_rotate_controller.h"
#include "../input_system/keyboard_input_controller.h"
#include "../managers/thread_state_manager.h" // 线程管理器
#include "../managers/game_entity_manager.h"

// libs
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif

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
#include <mutex>
#include <thread>
#include <stdexcept>
#include <condition_variable>
