// a vertex shader
// well ok, not really
#version 150

in vec4 position;
in vec4 color;

out vec4 fcolor;

void main() {
    fcolor = vec4(color.rgb, 1.0);
    gl_Position = vec4(position.xyz, 1.0);
}
