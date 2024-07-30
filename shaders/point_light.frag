
#version 460

layout (location = 0) in vec2 fragOffset;
layout (location = 0) out vec4 outColor;

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

const float M_PI = 3.1415926538;

void main()
{
	float dis = sqrt(dot(fragOffset, fragOffset));

	if (dis >= 1.0) // �����������Դ��Χ��ƬԪ
	{
		discard;
	}

	float cosDis = 0.5 * (cos(dis * M_PI) + 1.0); // ����� 1 -> 0 ��˥�����ӣ�ʵ�ֹ�Դ�����𽥼���
	outColor = vec4(push.color.xyz + 0.5 * cosDis, cosDis); // ���������ɫ
}
