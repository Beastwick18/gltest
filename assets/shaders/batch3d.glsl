#type vertex
#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in float aLight;
layout (location = 4) in float aSkyLight;

// uniform vec3 chunkPosition; // take in chunk position. "position" would then be relative to the chunk position

out vec2 vTexCoord;
out vec3 vNormal;
out float vLight;
out float vSkyLight;

uniform mat4 model;
uniform mat4 viewProj;

void main() {
    gl_Position = viewProj * model * vec4(aPosition, 1.0);
    vTexCoord = aTex;
    vNormal = aNormal;
    vLight = aLight;
    vSkyLight = aSkyLight;
}



#type fragment
#version 330 core

layout (location = 0) out vec4 outColor;

in vec2 vTexCoord;
in vec3 vNormal;
in float vLight;
in float vSkyLight;

uniform float skyBrightness;
uniform sampler2D tex0;

vec3 sunDirection = normalize(vec3(-0.27, 0.57, -0.57));
vec3 lightColor = vec3(1.0, 1.0, 1.0);
float ambientStrength = 0.6;

void main() {
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(sunDirection);
    float diff = max(dot(vNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    float light = max(vLight, vSkyLight * skyBrightness);
    outColor = vec4(min((diffuse + ambient), 1.0), 1.0) * texture(tex0, vTexCoord) * vec4(light, light, light, 1.0);
    if (outColor.a == 0) { discard; }
    // outColor = texture(tex0, vTexCoord) * vec4(light, light, light, 1.0);
    // float f = float(vLight);
    // float light = f / 15.0;
    // outColor = outColor * vec4(vLight, vLight, vLight, 1.0);
    // outColor[3] = texture(tex0, vTexCoord)[3];
}
