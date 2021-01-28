#version 420

uniform sampler2D   material_colormap;
uniform float       material_ambient;
uniform float       material_diffuse;
uniform float       material_specular;
uniform sampler2D   light_shadowmap;

in vec3 position;
in vec3 normal;
in vec2 texcoord;
in vec3 light_vector;

layout (location = 0) out vec4 fragment_color;

vec4 lighting()
{
    return vec4(1.0, 1.0, 1.0, 1.0);
}

void main()
{
    vec4 color = texture(material_colormap, texcoord);

    if (color.a < 0.004)
    {
        discard;
    }

    fragment_color = color * lighting();
}
