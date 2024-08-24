#version 450

#ifdef COMPILE_VERTEX_SHADER
/* VERTEX SHADER */

#define HAS_COLOR      0
#define HAS_CUSTOM_UV  1
#define HAS_TEXTURE    2
#define HAS_ANIMATION  3

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 mView;
    mat4 mProj;
    vec3 globalTint;

} ubo;

layout(push_constant) uniform PER_OBJECT
{
    mat4 mModel;
    vec4 colors[4];
    vec2 uvs[4];
    bool options[4];

} obj;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 globalTint;

vec2 positions[4] = vec2[](
    vec2(-0.5, -0.5),
    vec2( 0.5, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

vec2 uvs[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 0.0, -1.0),
    vec2( 0.0,  0.0),
    vec2(-1.0,  0.0)
);

void main()
{
    gl_Position = ubo.mProj * ubo.mView * obj.mModel * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    globalTint = ubo.globalTint;

    if (obj.options[HAS_COLOR])
        fragColor = obj.colors[gl_VertexIndex].rgb;
    else
        fragColor = vec3(1.0, 1.0, 1.0);

    if (obj.options[HAS_CUSTOM_UV])
        fragTexCoord = obj.uvs[gl_VertexIndex];
    else
        fragTexCoord = uvs[gl_VertexIndex];
}

#else
/* FRAGMENT SHADER */

layout(binding = 1) uniform sampler texSampler;
layout(binding = 2) uniform texture2D texInstance;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 globalTint;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(sampler2D(texInstance, texSampler), fragTexCoord);
}

#endif
