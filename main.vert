#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec3 vNormal;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

out vec2 fTexCoord;
out vec3 fNormal;
out vec3 fFragPos;

vec4 position = vec4(vPosition, 1.);
mat4 modelViewMatrix = uViewMatrix * uModelMatrix;
mat4 modelViewProjectionMatrix = uProjectionMatrix * modelViewMatrix;
mat3 normalMatrix = mat3(transpose(inverse(uModelMatrix)));

void main()
{
    fTexCoord = vTexCoord;
    fNormal = normalMatrix * vNormal;
    fFragPos = vPosition;

    //gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(vPosition, 1.);
    gl_Position = modelViewProjectionMatrix * position;
}
