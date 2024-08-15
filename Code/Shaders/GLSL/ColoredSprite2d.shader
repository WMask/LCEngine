#version 450

#ifdef COMPILE_VERTEX_SHADER

layout(push_constant) uniform PER_OBJECT
{
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
    vec4 colors[4];

} obj;

layout(location = 0) out vec3 fragColor;

vec2 positions[4] = vec2[](
    vec2(-0.5, -0.5),
    vec2( 0.5, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

void main()
{
    gl_Position = obj.mProj * obj.mView * obj.mModel * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = obj.colors[gl_VertexIndex].rgb;
}

#else /* FRAGMENT SHADER */

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(fragColor, 1.0);
}

#endif
