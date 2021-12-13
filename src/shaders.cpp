//#ifndef TEST_GLSL
//#define TEST_GLSL

//#include "shaders.h"

//namespace MinecraftClone {
//    const char *fragmentSource = R"glsl(
//        #version 330 core
        
//        out vec4 outColor;
        
//        in vec2 texCoord;
        
//        uniform sampler2D tex0;
//        uniform float test;
        
//        void main() {
//            float lerpY = test * gl_FragCoord.y / 720.0f;
//            float lerpX = test * gl_FragCoord.x / 720.0f;
//            outColor = texture(tex0, texCoord) * mix(mix(vec4(test, 1-(test), 0, 1.0), vec4(0, test, 1-(test), 1.0), lerpX), vec4(1-(test), 0, test, 1.0), lerpY);
            
//            // float lerpY = gl_FragCoord.y / 720.0f;
//            // float lerpX = gl_FragCoord.x / 720.0f;
//            // outColor = mix(mix(vec4(test, 1-(test), 0, 1.0), vec4(0, test, 1-(test), 1.0), lerpX), vec4(1-(test), 0, test, 1.0), lerpY);
//            //outColor = mix(mix(vec4(1.0, 1.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), lerpY), vec4(0.0, 1.0, 1.0, 1.0), lerpX);
//            //outColor = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), test);
//        }
//    )glsl";
    
//    const char *vertexSource = R"glsl(
//        #version 330 core
        
//        layout (location = 0) in vec2 position;
//        layout (location = 1) in vec2 aTex;
        
//        out vec2 texCoord;
        
//        void main() {
//            gl_Position = vec4(position, 1.0, 1.0);
//            texCoord = aTex;
//        }
//    )glsl";
//}

//#endif
