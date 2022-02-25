#type vertex
#version 430 core

layout (location = 0) in vec3 aPosition;
// layout (location = 1) in vec3 aNormal;
layout (location = 1) in vec2 aTexCoord;
// layout (location = 3) in int aLight;
layout (location = 2) in uint aData;
// layout (location = 3) in float aLight;
// layout (location = 4) in float aSkyLight;

// uniform vec3 chunkPosition; // take in chunk position. "position" would then be relative to the chunk position

out vec2 vTexCoord;
out vec3 vNormal;
flat out uint vData;
// out float vLight;
// out float vSkyLight;

uniform mat4 model;
uniform mat4 viewProj;

void main() {
    gl_Position = viewProj * model * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
    // vNormal = aNormal;
    vData = aData;
}



#type fragment
#version 430 core

layout (location = 0) out vec4 outColor;

in vec2 vTexCoord;
// in vec3 vNormal;
flat in uint vData;
// in float vLight;
// in float vSkyLight;

uniform float skyBrightness;
uniform sampler2D selTex;

vec3 sunDirection = normalize(vec3(-0.27, 0.57, -0.57));
vec3 lightColor = vec3(1.0, 1.0, 1.0);
// vec3 lightCol = vec3(1.f, .0, .0f);
vec3 lightCol = vec3(1.f, .95f, .6f);
float ambientStrength = 0.6;

const vec3 normals[] = vec3[](
    vec3(1f, 0f, 0f),
    vec3(0f, 1f, 0f),
    vec3(0f, 0f, 1f),
    vec3(-1f, 0f, 0f),
    vec3(0f, -1f, 0f),
    vec3(0f, 0f, -1f)
);

void main() {
    float light = (float( vData & 0xF ) + .5f) / 15.5f;
    float skyLight = (float( ( vData & 0xF0 ) >> 4 ) + .5f) / 15.5f;
    uint normalIndex = (vData & 0x700) >> 8;
    
    vec4 tex = texture(selTex, vTexCoord);
    if(tex.a == 0)
        discard;
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(sunDirection);
    float diff = max(dot(normals[normalIndex], lightDir), 0.0);
    // float diff = 1.0f;
    vec3 diffuse = diff * lightColor;
    // // This works for colored lighting
    // vec3 light = mix(vLight*lightCol, vSkyLight * skyBrightness * lightColor, .5);
    
    vec3 maxLight = max(light*lightCol, skyLight * lightColor* skyBrightness);
    outColor = vec4(min((diffuse + ambient), 1.0) * tex.rgb * maxLight, tex.a);
}
