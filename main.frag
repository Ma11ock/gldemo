#version 450 core

uniform sampler2D uTexUnit;

in vec4 fColor;
in vec2 fTexCoord;

layout (location = 0) out vec4 fragColor;

void main()
{
    vec4 objectColor = texture(uTexUnit, fTexCoord);
    fragColor = objectColor;
}
