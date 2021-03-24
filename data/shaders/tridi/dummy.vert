#version 420

uniform mat4 full_transform;

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;

out vec2 texcoord;

void main()
{
    texcoord = vertex_texcoord;

    gl_Position = vec4(vertex_position, 1.0) * full_transform;
}
