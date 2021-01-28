#version 420

uniform sampler2D   material_colormap;
uniform float       material_diffuse;
uniform float       material_specular;
uniform vec3        light_color;
uniform float       light_ambient;
uniform float       light_diffuse;
uniform float       light_specular;
uniform float       light_focus;
uniform sampler2D   light_shadowmap;

in vec3 normal;
in vec2 texcoord;
in vec3 eye_vector;
in vec3 light_vector;

layout (location = 0) out vec4 fragment_color;

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

float diffuse_lighting(vec3 l, vec3 n)
{
    float factor = dot(l, n) * light_diffuse;

    if (factor < 0)
    {
        factor = 0;
    }

    return light_ambient + factor * material_diffuse;
}

vec3 specular_lighting(vec3 l, vec3 n)
{
    vec3 r = normalize(eye_vector);
    vec3 v = reflect(r, n);

    float factor = 0;
    float s = dot(l, v);

    if (s > 0)
    {
        factor = pow(s, light_focus) * light_specular;
    }

    return light_color * factor * material_specular;
}

void shading()
{
    vec3 lightcoord = solve_uvmap(-light_vector);

    float shadow = texture(light_shadowmap, lightcoord.xy);

    if (lightcoord.z > shadow)
    {
        discard;
    }
}

void main()
{
    vec4 color = texture(material_colormap, texcoord);

    if (color.a < 0.004)
    {
        discard;
    }

    shading();

    vec3 black(0.0, 0.0, 0.0);
    vec3 white(1.0, 1.0, 1.0);

    vec3 l = normalize(light_vector);
    vec3 n = normalize(normal);

    vec3 diffuse = diffuse_lighting(l, n);
    vec3 specular = specular_lighting(l, n);

    vec3 lighted = clamp(color.rgb * diffuse + specular, black, white);

    fragment_color = vec4(lighted, color.a);
}
