#version 420

uniform sampler2D material_colormap;

in vec2 texcoord;

layout (location = 0) out vec4 fragment_color;

void main()
{
    fragment_color = texture(material_colormap, texcoord);
}
