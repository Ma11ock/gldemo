#version 450 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;

    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
};

uniform Light uLight;

uniform Material uMaterial;

uniform mat4 uTextureMatrix;
// Color transformation matrix.
uniform mat4 uColorMatrix;
// Camera position
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
    // Ambient.
    vec3 ambient = uLight.ambientColor * uMaterial.ambient;

    // Diffuse.
    vec3 lightDir = normalize(uLight.position - fFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uLight.diffuseColor * (diff * uMaterial.diffuse);

    // Specular.
    vec3 viewDir = normalize(uViewPos - fFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), uMaterial.shininess);
    vec3 specular = uLight.specularColor * (spec * uMaterial.specular);


    return vec4(ambient + diffuse + specular, 1.);
}

void main()
{
    vec4 objectColor = texture(uTexUnit, fTexCoord);
    fragColor = doLights() * objectColor;
}
