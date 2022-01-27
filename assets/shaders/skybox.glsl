#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}  

#type fragment
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform float time;
uniform samplerCube skybox;

// const vec4 skytop = vec4(0.0f, 0.0f, 1.0f, 1.0f);
const vec4 skytop = vec4(0.01f, 0.66f, 0.96f, 1.0f);
// const vec4 skyhorizon = vec4(0.3294f, 0.92157f, 1.0f, 1.0f);
const vec4 skyhorizon = vec4(0.92157f, 0.92157f, 1.0f, 1.0f);
const vec4 night = vec4(0.0f, 0.0f, 0.0f, 1.0f);
const vec4 sunsetColor = vec4(0.99f, 0.37f, 0.33f, 1.0f);


void main()
{    
    vec3 pos = normalize(TexCoords);
    float s= .7f * exp(-pow((10 * time - 2) * (10 * time - 2), 2));
    // float s= .7f * exp(-(10 * (time - .2)));
    // float s = 0;
    // // FragColor = vec4((1-pos.y) * 1, (1-pos.y) * 1, 1.0, 1.0);
    // vec4 sunset = mix(skytop, sunsetColor, pos.y), 
    // FragColor = mix(night, mix(mix(skyhorizon, skytop, pos.y), sunset, s), time);
    vec4 horizon = mix(skyhorizon, sunsetColor, s);
    FragColor = mix(night, mix(horizon, skytop, pos.y+.1f * (1-s)), time);
}
