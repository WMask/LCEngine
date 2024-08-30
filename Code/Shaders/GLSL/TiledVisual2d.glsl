#version 450

#ifdef COMPILE_VERTEX_SHADER
/* VERTEX SHADER */

layout(set = 0, binding = 0) uniform UniformBufferObject{
	mat4 mView;
	mat4 mProj;
	vec3 globalTint;

} ubo;

layout(push_constant) uniform PER_OBJECT{
	mat4 mModel;

} obj;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 globalTint;

void main()
{
	gl_Position = ubo.mProj * ubo.mView * obj.mModel * vec4(position, 1.0);
	fragTexCoord = texCoord;
	globalTint = ubo.globalTint;
}

#else
/* FRAGMENT SHADER */

layout(set = 0, binding = 1) uniform sampler texSampler;
layout(set = 1, binding = 0) uniform texture2D texInstance;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 globalTint;

layout(location = 0) out vec4 outColor;

void main()
{
	vec4 texColor = texture(sampler2D(texInstance, texSampler), fragTexCoord);
	vec4 tint = vec4(globalTint, 1.0);
	outColor = texColor * tint;
}

#endif
