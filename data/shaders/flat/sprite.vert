#version 420

uniform vec2 mesh_position;
uniform vec2 mesh_scale;

layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 vertex_texcoord;

out vec2 position;
out vec2 texcoord;

void main()
{
    position = mesh_position + vertex_position * mesh_scale;
    texcoord = vertex_texcoord;

    gl_Position = vec4(position, 0.0, 1.0);
}
