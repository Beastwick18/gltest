#version 330 core

in vec2 position;

void main() {
    gl_Position = vec4(position.x * 2, position.y, 0.0, 2.0);
}

