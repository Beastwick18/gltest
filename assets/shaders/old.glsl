#type vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 aTex;

out vec2 texCoord;

// uniform mat4 MVP;
uniform mat4 model;
uniform mat4 viewProj;

void main() {
    gl_Position = viewProj * model * vec4(position, 1.0);
    texCoord = aTex;
}



#type fragment
#version 330 core

layout (location = 0) out vec4 outColor;

in vec2 texCoord;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform float test;

void main() {
    // float lerpX = test * texCoord.x;
    // float lerpY = test * texCoord.y;
    outColor = texture(tex0, texCoord);
    // Mix texture 0 and texture 1 together
    // outColor = mix(texture(tex0, texCoord), texture(tex1, texCoord), 0.5);
    // Mix in some cool colors
    // outColor = mix(outColor, mix(mix(vec4(test, 1-(test), 0, 1.0), vec4(0, test, 1-(test), 1.0), lerpX), vec4(1-(test), 0, test, 1.0), lerpY), .5);
    
    // outColor = mix(mix(vec4(test, 1-(test), 0, 0.1), vec4(0, test, 1-(test), 0.1), lerpX), vec4(1-(test), 0, test, 0.1), lerpY);
    // outColor = texture(tex0, texCoord) * mix(mix(vec4(test, 1-(test), 0, 1.0), vec4(0, test, 1-(test), 1.0), lerpX), vec4(1-(test), 0, test, 1.0), lerpY);
    
    // float lerpY = gl_FragCoord.y / 720.0f;
    // float lerpX = gl_FragCoord.x / 720.0f;
    // outColor = mix(mix(vec4(test, 1-(test), 0, 1.0), vec4(0, test, 1-(test), 1.0), lerpX), vec4(1-(test), 0, test, 1.0), lerpY);
    //outColor = mix(mix(vec4(1.0, 1.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), lerpY), vec4(0.0, 1.0, 1.0, 1.0), lerpX);
    //outColor = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), test);
}
