#type vertex
#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

// uniform vec3 chunkPosition; // take in chunk position. "position" would then be relative to the chunk position

out vec2 vTexCoord;
out vec3 vNormal;

uniform mat4 model;
uniform mat4 viewProj;
uniform float waveOffset;

float waveHeight = .05;

void main() {
    float height = aPosition.y+ waveHeight*sin(aPosition.x + aPosition.z + waveOffset);
    gl_Position = viewProj * model * vec4(aPosition.x, height, aPosition.z, 1.0);
    vTexCoord = aTex;
    vNormal = aNormal;
}

#type fragment
#version 330 core

layout (location = 0) out vec4 outColor;

in vec2 vTexCoord;
in vec3 vNormal;

uniform sampler2D tex0;

vec3 sunDirection = normalize(vec3(-0.27, 0.57, -0.57));
vec3 lightColor = vec3(1.0, 1.0, 1.0);
float ambientStrength = .4;

void main() {
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(sunDirection);
    float diff = max(dot(vNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    outColor = vec4(min((diffuse + ambient), 1.0), 1.0) * texture(tex0, vTexCoord);
}