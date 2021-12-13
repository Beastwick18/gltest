#version 330 core

out vec4 outColor;

in vec2 texCoord;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float test;

void main() {
    float lerpX = test * texCoord.x;
    float lerpY = test * texCoord.y;
    // Multpily pixel color value by some mixed value
    outColor = mix(texture(tex0, texCoord), texture(tex1, texCoord), .5) * mix(mix(vec4(test, 1-(test), 0, 1.0), vec4(0, test, 1-(test), 1.0), lerpX), vec4(1-(test), 0, test, 1.0), lerpY);
    // outColor = texture(tex0, texCoord) * mix(mix(vec4(test, 1-(test), 0, 1.0), vec4(0, test, 1-(test), 1.0), lerpX), vec4(1-(test), 0, test, 1.0), lerpY);
    
    // float lerpY = gl_FragCoord.y / 720.0f;
    // float lerpX = gl_FragCoord.x / 720.0f;
    // outColor = mix(mix(vec4(test, 1-(test), 0, 1.0), vec4(0, test, 1-(test), 1.0), lerpX), vec4(1-(test), 0, test, 1.0), lerpY);
    //outColor = mix(mix(vec4(1.0, 1.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), lerpY), vec4(0.0, 1.0, 1.0, 1.0), lerpX);
    //outColor = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), test);
}
