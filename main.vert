#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec3 vNormal;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uModelViewProjectionMatrix;
uniform mat3 uNormalMatrix;

out vec2 fTexCoord;
out vec3 fNormal;
out vec3 fFragPos;

vec4 position = vec4(vPosition, 1.);

void main()
{
    fTexCoord = vTexCoord;
    fNormal = uNormalMatrix * vNormal;
    fFragPos = vPosition;

    gl_Position = uModelViewProjectionMatrix * position;
}
