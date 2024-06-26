
#version 460

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 fragUV;

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

layout(set = 0, binding = 1) uniform sampler2D image;

layout(push_constant) uniform Push
{
	mat4 modelMatrix;
	mat4 normalMatrix;

} push;

void main()
{
	vec3 diffuseLight = ubo.ambient_light_color.xyz * ubo.ambient_light_color.w;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPosWorld = ubo.inverse_view[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

	for (int i = 0; i < ubo.num_lights; i++)
	{
		PointLight light = ubo.point_lights[i];
		vec3 directionToLight = light.position.xyz - fragPosWorld;
		float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
		directionToLight = normalize(directionToLight);

		float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;

		diffuseLight += intensity * cosAngIncidence;

		// specular lighting
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 512.0); // higher values -> sharper highlight
		// specularLight += intensity * blinnTerm;
		specularLight = intensity; // 此处不使用高光
	}

	vec3 imageColor = texture(image, fragUV * 4).rgb;

	outColor = vec4((diffuseLight * imageColor + specularLight * imageColor), 1.0);
}
