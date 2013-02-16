// a vertex shader
// well ok, not really
#version 150

in vec3 position;

void main() {
    gl_Position = vec4(position, 1.0);
}
