
#include "math_tools.h"

namespace vulkancraft
{
	template<typename T>
	glm::vec3 move_towards(const glm::vec3& current, const glm::vec3& target, T max_distance)
	{
		glm::vec3 direction = target - current;
		float distance = glm::length(direction);

		if (distance <= max_distance) return target;
		else
		{
			glm::vec3 normalizedDirection = glm::normalize(direction);
			return current + normalizedDirection * max_distance;
		}
	}
}
