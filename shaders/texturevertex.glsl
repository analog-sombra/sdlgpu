#version 450

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texCoord;

layout(location = 1) out vec2 v_texCoord;

layout(set = 1, binding = 0) uniform Transform
{
    mat4 model;
};

void main()
{
    gl_Position = model * vec4(a_position, 1.0);
    v_texCoord = a_texCoord;
}
