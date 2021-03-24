#version 420

uniform sampler2D mesh_texture;

in vec2 position;
in vec2 texcoord;

layout (location = 0) out vec4 fragment_color;

void main()
{
    fragment_color = texture(mesh_texture, texcoord);

    if (fragment_color.a < 0.004)
    {
        discard;
    }
}
