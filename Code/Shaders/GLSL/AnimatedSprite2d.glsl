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
	vec4 colors[4];
	float anim[4];
	float hasColor;

} obj;

layout(location = 0) out vec2  fragTexCoord;
layout(location = 1) out vec4  fragColor;
layout(location = 2) out vec3  fragGlobalTint;

vec2 positions[4] = vec2[](
	vec2(-0.5, -0.5),
	vec2( 0.5, -0.5),
	vec2( 0.5,  0.5),
	vec2(-0.5,  0.5)
);

vec2 uvs[4] = vec2[](
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
	vec2(0.0, 1.0)
);

void main()
{
	gl_Position = ubo.mProj * ubo.mView * obj.mModel * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragGlobalTint = ubo.globalTint;

	if (obj.hasColor > 0.5)
		fragColor = obj.colors[gl_VertexIndex];
	else
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);

	float frameOffsetX = obj.anim[2];
	float frameOffsetY = obj.anim[3];

	fragTexCoord = vec2(uvs[gl_VertexIndex].x * obj.anim[0] + frameOffsetX, uvs[gl_VertexIndex].y * obj.anim[1] + frameOffsetY);
}

#else
/* FRAGMENT SHADER */

layout(set = 0, binding = 1) uniform sampler texSampler;
layout(set = 1, binding = 0) uniform texture2D texInstance;

layout(location = 0) in vec2  texCoord;
layout(location = 1) in vec4  color;
layout(location = 2) in vec3  globalTint;

layout(location = 0) out vec4 outColor;

void main()
{
	vec4 texColor = texture(sampler2D(texInstance, texSampler), texCoord);
	vec4 tint = vec4(globalTint, 1.0);

	outColor = texColor * color * tint;
}

#endif
