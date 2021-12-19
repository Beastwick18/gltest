#type vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 aTex;

out vec2 texCoord;

void main() {
    gl_Position = vec4(position, 1.0);
    texCoord = aTex;
}

#type fragment
#version 330 core

layout (location = 0) out vec4 outColor;

in vec2 texCoord;

uniform sampler2D tex0;

void main() {
    outColor = texture(tex0, texCoord);
    // outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
