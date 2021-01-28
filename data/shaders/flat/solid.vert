#version 420

uniform vec2 mesh_position;
uniform vec2 mesh_scale;

layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec4 vertex_color;

out vec2 position;
out vec4 color;

void main()
{
    position = mesh_position + vertex_position * mesh_scale;
    color = vertex_color;

    gl_Position = vec4(position, 0.0, 1.0);
}
