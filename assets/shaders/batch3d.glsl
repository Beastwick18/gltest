#type vertex
#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in int aLight;
// layout (location = 3) in float aLight;
// layout (location = 4) in float aSkyLight;

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
    vLight = (float( aLight & 0xF ) + .5f) / 15.5f;
    vSkyLight = (float( ( aLight & 0xF0 ) >> 4 ) + .5f) / 15.5f;
}



#type fragment
#version 430 core

layout (location = 0) out vec4 outColor;

in vec2 vTexCoord;
in vec3 vNormal;
in float vLight;
in float vSkyLight;

uniform float skyBrightness;
uniform sampler2D tex0;

vec3 sunDirection = normalize(vec3(-0.27, 0.57, -0.57));
vec3 lightColor = vec3(1.0, 1.0, 1.0);
vec3 lightCol = vec3(1.0, 0.0, 0.0);
float ambientStrength = 0.6;

void main() {
    vec4 tex = texture(tex0, vTexCoord);
    if(tex.a == 0)
        discard;
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(sunDirection);
    float diff = max(dot(vNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    // // This works for colored lighting
    // vec3 light = mix(vLight*lightCol, vSkyLight * skyBrightness * lightColor, .5);
    float light = max(vLight, vSkyLight * skyBrightness);
    outColor = vec4(min((diffuse + ambient), 1.0) * tex.rgb * light, tex.a);
}
