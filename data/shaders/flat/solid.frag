#version 420

in vec2 position;
in vec4 color;

layout (location = 0) out vec4 fragment_color;

void main()
{
    fragment_color = color;

    if (fragment_color.a < 0.004)
    {
        discard;
    }
}
