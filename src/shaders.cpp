#ifndef TEST_GLSL
#define TEST_GLSL

#include "shaders.h"

namespace MinecraftClone {
    const char *fragmentSource = R"glsl(
        #version 330 core
        
        uniform float test;
        
        out vec4 outColor;
        
        void main() {
            // float lerpY = test * gl_FragCoord.y / 720.0f;
            // float lerpX = test * gl_FragCoord.x / 720.0f;
            float lerpY = gl_FragCoord.y / 720.0f;
            float lerpX = gl_FragCoord.x / 720.0f;
            outColor = mix(mix(vec4(test, 1-(test), 0, 1.0), vec4(0, test, 1-(test), 1.0), lerpX), vec4(1-(test), 0, test, 1.0), lerpY);
            //outColor = mix(mix(vec4(1.0, 1.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), lerpY), vec4(0.0, 1.0, 1.0, 1.0), lerpX);
            //outColor = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), test);
        }
    )glsl";
    
    const char *vertexSource = R"glsl(
        #version 330 core
        
        in vec2 position;
        
        void main() {
            gl_Position = vec4(position.x, position.y, 0.0, 2.0);
        }
    )glsl";
}

#endif
