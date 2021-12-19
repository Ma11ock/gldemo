#version 450 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular; 
    float shininess;
};

struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_NUM_LIGHTS 16

uniform DirLight uDirLights[MAX_NUM_LIGHTS];

uniform PointLight uPointLights[MAX_NUM_LIGHTS];

uniform SpotLight uSpotLights[MAX_NUM_LIGHTS];

uniform uint uNumDirLights;
uniform uint uNumPointLights;
uniform uint uNumSpotLights;

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
vec3 normal = normalize(fNormal);
// Texture color.
vec4 diffuseColor = texture(uMaterial.diffuse, fTexCoord);
vec4 specularColor = texture(uMaterial.specular, fTexCoord);
vec3 viewDir = normalize(uViewPos - fFragPos);

vec4 doPointLight(PointLight light)
{
    vec3 lightDir = normalize(light.position - fFragPos);

    float diff = max(dot(normal, lightDir), 0.f);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), uMaterial.shininess);

    float dis = length(light.position - fFragPos);
    float attuentation = 1.f / (light.constant + light.linear * dis +
        light.quadratic * (dis * dis));

    // Combine results.
    vec3 ambient = (light.ambient * diffuseColor.rgb) * attuentation;
    vec3 diffuse = (light.diffuse * diff * diffuseColor.rgb) * attuentation;
    vec3 specular = (light.specular * spec * specularColor.rgb) * attuentation;

    return vec4(ambient + diffuse + specular, 1.);
}

vec4 doDirLight(DirLight light)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    // combine results
    vec3 ambient = light.ambient * diffuseColor.rgb;
    vec3 diffuse = light.diffuse * diff * diffuseColor.rgb;
    vec3 specular = light.specular * spec * specularColor.rgb;
    return vec4(ambient + diffuse + specular, 1.);
}

vec4 doSpotLight(SpotLight light)
{
    vec3 lightDir = normalize(light.position - fFragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    // attenuation
    float dist = length(light.position - fFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist +
                               light.quadratic * (dist * dist));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * diffuseColor.rgb;
    vec3 diffuse = light.diffuse * diff * diffuseColor.rgb;
    vec3 specular = light.specular * spec * specularColor.rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return vec4(ambient + diffuse + specular, 1.);
}

void main()
{
    vec4 result = vec4(0., 0., 0., 0.);
    for(uint i = 0; i < uNumDirLights; i++)
        result += doDirLight(uDirLights[i]);

    for(uint i = 0; i < uNumPointLights; i++)
        result += doPointLight(uPointLights[i]);

    for(uint i = 0; i < uNumSpotLights; i++)
        result += doSpotLight(uSpotLights[i]);

    fragColor = result;
}
