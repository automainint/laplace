#version 420

uniform mat3 object_rotation;
uniform mat4 object_transform;
uniform mat4 view_transform;
uniform vec3 eye_position;
uniform vec3 light_position;

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;

out vec3 normal;
out vec2 texcoord;
out vec3 eye_vector;
out vec3 light_vector;

void main()
{
    vec4 p = vec4(vertex_position, 1.0) * object_transform;
    vec4 n = vec4(vertex_normal, 1.0) * object_rotation;

    gl_Position = p * view_transform;

    normal = n.xyz;
    texcoord = vertex_texcoord;
    eye_vector = eye_position - p.xyz;
    light_vector = light_position - p.xyz;
}
