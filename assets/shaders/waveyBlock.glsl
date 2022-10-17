#type vertex
#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in uint aData;

// uniform vec3 chunkPosition; // take in chunk position. "position" would then be relative to the chunk position

out vec2 vTexCoord;
flat out uint vData;
smooth out vec4 ioEyeSpacePosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float waveOffset;

float waveHeight = .05;

void main() {
    mat4 mvMatrix = view * model;
    float height = aPosition.y;
    // float height = aPosition.y+ waveHeight*sin(aPosition.x + aPosition.z + waveOffset);
    gl_Position = proj * mvMatrix * vec4(aPosition.x, height, aPosition.z, 1.0);
    vTexCoord = aTexCoord;
    vData = aData;
    ioEyeSpacePosition = mvMatrix * vec4(aPosition, 1.0);
}

#type fragment
#version 430 core

layout (location = 0) out vec4 outColor;

in vec2 vTexCoord;
flat in uint vData;
smooth in vec4 ioEyeSpacePosition;

uniform float skyBrightness;
uniform sampler2D selTex;

vec3 sunDirection = normalize(vec3(-0.27, 0.57, -0.57));
vec3 skyLightColor = vec3(1.0, 1.0, 1.0);
vec3 lightColor = vec3(1.f, .95f, .6f);
// vec3 lightColor = vec3(0.0f, .05f, .6f);
float ambientStrength = 0.6;
const vec4 sunsetColor = vec4(0.99f, 0.37f, 0.33f, 1.0f);
const vec4 night = vec4(0.0f, 0.0f, 0.0f, 1.0f);

const vec3 normals[] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(-1.0, 0.0, 0.0),
    vec3(0.0, -1.0, 0.0),
    vec3(0.0, 0.0, -1.0)
);

struct FogParameters {
	vec3 color;
	float linearStart;
	float linearEnd;
	float density;
	
	int equation;
	bool isEnabled;
};

float getFogFactor(FogParameters params, float fogCoordinate) {
	float result = 0.0;
	if(params.equation == 0)
	{
		float fogLength = params.linearEnd - params.linearStart;
		result = (params.linearEnd - fogCoordinate) / fogLength;
	}
	else if(params.equation == 1) {
		result = exp(-params.density * fogCoordinate);
	}
	else if(params.equation == 2) {
		result = exp(-pow(params.density * fogCoordinate, 2.0));
	}
	
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

// const FogParameters fogParams = {vec3(0.85, 0.85, 0.95), 20.0, 80.0, 0.12, 2, true};
// uniform FogParameters fogParams = {vec3(0.25, 0.25, 0.95), 20.0, 80.0, 0.12, 2, true};
uniform FogParameters fogParams;

void main() {
    vec4 tex = texture(selTex, vTexCoord);
    if(tex.a == 0)
        discard;
    
    float light = (float( vData & 0xF ) + 2.0f) / 17.0f;
    float skyLight = (float( ( vData & 0xF0 ) >> 4 ) + 2.0f) / 17.0f;
    skyLight = skyLight * skyLight;
    light = light * light;
    uint normalIndex = (vData & 0x700) >> 8;
    
    vec3 ambient = ambientStrength * skyLightColor;
    vec3 lightDir = normalize(sunDirection);
    float diff = max(dot(normals[normalIndex], lightDir), 0.0);
    // float diff = 1.0f;
    vec3 diffuse = diff * skyLightColor;
    // // This works for colored lighting
    // vec3 light = mix(vLight*lightColor, vSkyLight * skyBrightness * skyLightColor, .5);
    
    vec3 maxLight = max(light*lightColor, skyLight * skyLightColor* skyBrightness);
    outColor = vec4(min((diffuse + ambient), 1.0) * tex.rgb * maxLight, tex.a);
    if(fogParams.isEnabled) {
        float s= .7f * exp(-pow((10 * skyBrightness - 2) * (10 * skyBrightness - 2), 2));
        vec4 horizon = mix(vec4(fogParams.color, 1.0), sunsetColor, s);
        vec4 fogColor = mix(night, horizon, skyBrightness);
        float fogCoordinate = abs(ioEyeSpacePosition.z / ioEyeSpacePosition.w);
        float factor = getFogFactor(fogParams, fogCoordinate);
        outColor = mix(outColor, fogColor, factor);
        // outColor.a = outColor.a * 1-factor;
    }
}
