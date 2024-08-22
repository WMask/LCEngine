#version 450

#ifdef COMPILE_VERTEX_SHADER
/* VERTEX SHADER */

layout(binding = 0) uniform UniformBufferObject {
    mat4 mView;
    mat4 mProj;
    vec3 globalTint;

} ubo;

layout(push_constant) uniform PER_OBJECT
{
    mat4 mModel;
    vec4 colors[4];

} obj;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 globalTint;

vec2 positions[4] = vec2[](
    vec2(-0.5, -0.5),
    vec2( 0.5, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

void main()
{
    gl_Position = ubo.mProj * ubo.mView * obj.mModel * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = obj.colors[gl_VertexIndex].rgb;
    globalTint = ubo.globalTint;
}

#else
/* FRAGMENT SHADER */

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 globalTint;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(fragColor * globalTint, 1.0);
}

#endif
