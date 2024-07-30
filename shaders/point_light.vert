
#version 460

const vec2 OFFSETS[6] = vec2[](
	vec2(-1.0, -1.0),
	vec2(-1.0, 1.0),
	vec2(1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(-1.0, 1.0),
	vec2(1.0, 1.0)
);

layout (location = 0) out vec2 fragOffset;

struct PointLight
{
	vec4 position; // ignore w
	vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projection;
	mat4 view;
	mat4 inverse_view;
	vec4 ambient_light_color; // w is intensity
	PointLight point_lights[256];
	int num_lights;

} ubo;

layout(push_constant) uniform Push
{
	vec4 position;
	vec4 color;
	float radius;

} push;

void main()
{
	fragOffset = OFFSETS[gl_VertexIndex];
	vec3 cameraRightWorld = {ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]};
	vec3 cameraUpWorld = {ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]};

	// 用光源中心位置，光源范围配合摄像机计算圆形点光源范围
	vec3 positionWorld = push.position.xyz
	+ push.radius * fragOffset.x * cameraRightWorld
	+ push.radius * fragOffset.y * cameraUpWorld;

	gl_Position = ubo.projection * ubo.view * vec4(positionWorld, 1.0);
}
