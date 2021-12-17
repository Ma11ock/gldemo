#version 450 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular; 
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

in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fFragPos;

layout (location = 0) out vec4 fragColor;

// Normal vector.
vec3 norm = normalize(fNormal);
// Texture color.
vec4 diffuseColorT = texture(uMaterial.diffuse, fTexCoord);
vec4 specularColorT = texture(uMaterial.specular, fTexCoord);

vec4 doLights()
{
    // Ambient.
    vec3 ambient = uLight.ambientColor * diffuseColorT.rgb;

    // Diffuse.
    vec3 lightDir = normalize(uLight.position - fFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uLight.diffuseColor * diff * diffuseColorT.rgb;

    // Specular.
    vec3 viewDir = normalize(uViewPos - fFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), uMaterial.shininess);
    vec3 specular = uLight.specularColor * spec * specularColorT.rgb;


    return vec4(ambient + diffuse + specular, 1.);
}

void main()
{
    fragColor = doLights();
}
