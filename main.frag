#version 450 core

uniform sampler2D uTexUnit;
uniform float uAmbientStrength;
uniform vec3 uLightPos;

in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fFragPos;

layout (location = 0) out vec4 fragColor;

void main()
{
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(uLightPos - fFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightDir;
    vec4 objectColor = texture(uTexUnit, fTexCoord);
    fragColor = (uAmbientStrength + vec4(diffuse, 1.)) * objectColor;
}
