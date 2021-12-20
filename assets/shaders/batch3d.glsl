#type vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 aTex;

// uniform vec3 chunkPosition; // take in chunk position. "position" would then be relative to the chunk position

out vec2 texCoord;

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

void main() {
    outColor = texture(tex0, texCoord);
}
