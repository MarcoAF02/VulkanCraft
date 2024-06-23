
#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL // 下面这个头文件是实验性的 GLM 扩展
#include <glm/gtx/compatibility.hpp> // 包含线性插值，球面插值等工具

namespace vulkancraft
{
	// 用法同 Unity 引擎的 MoveTowards()
	template<typename T>
	glm::vec3 move_towards(const glm::vec3& current, const glm::vec3& target, T max_distance);
}
