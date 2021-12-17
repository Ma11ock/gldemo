#version 450 core

// Color of the light.
uniform vec3 uLightColor;
// Specular Light strength.
uniform float uSpecularStrength;
// Ambient light strength.
uniform float uAmbientStrength;
// Position of the light.
uniform vec3 uLightPos;
// Texture transformation matrix.
uniform mat4 uTextureMatrix;
// Color transformation matrix.
uniform mat4 uColorMatrix;
// Camera position.
uniform vec3 uViewPos;
// Texture.
uniform sampler2D uTexUnit;

in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fFragPos;

layout (location = 0) out vec4 fragColor;

vec3 norm = normalize(fNormal);

vec4 doLights()
{
    // Data we need.
    vec3 lightDir = normalize(uLightPos - fFragPos);
    vec3 viewDir = normalize(uViewPos - fFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient.
    vec3 ambient = uAmbientStrength * uLightColor;

    // Specular.
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.), 32)
        * uSpecularStrength
        * uLightColor;

    // Diffuse.
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    return vec4(uAmbientStrength + diffuse + specular, 1.);
}

void main()
{
    vec4 objectColor = texture(uTexUnit, fTexCoord);
    fragColor = doLights() * objectColor;
}
