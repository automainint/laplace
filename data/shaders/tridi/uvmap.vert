#version 420

uniform vec3    center_position;
uniform mat3    object_rotation;
uniform mat4    object_transform;
uniform vec3    light_position;
uniform vec3    light_color;
uniform float   light_intensity;

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texcoord;

out vec3 position;
out vec3 normal;
out vec2 texcoord;
out vec3 light_vector;

float sovle_u(float x, float y)
{
    return atan(y, x) * 0.31830988618;
}

float solve_v(float z)
{
    return asin(z) * 0.63661977236;
}

vec3 solve_uvmap(vec3 r)
{
    float depth = length(r);
    float u = 0;
    float v = 0;

    if (depth > 0)
    {
        vec3 n = r / depth;
        u = solve_y(n.x, n.y);
        v = solve_v(n.z);
    }

    return vec3(u, v, depth);
}

void main()
{
    vec4 p = vec4(vertex_position, 1.0) * object_transform;
    vec4 n = vec4(vertex_normal, 1.0) * object_rotation;
    vec3 r = p.xyz - center_position;

    gl_Position = vec4(solve_uvmap(r), 1.0);

    position = p.xyz;
    normal = n.xyz;
    texcoord = vertex_texcoord;
    light_vector = position - light_position;
}
