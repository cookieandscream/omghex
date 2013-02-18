// a vertex shader
// well ok, not really
#version 150

in vec4 position;
in vec4 color;

out vec4 fcolor;

uniform mat4 modelview;
uniform mat4 projection;

void main() {
    fcolor = vec4(color.rgb, 1.0);
    gl_Position = projection * modelview * vec4(position.xyz, 1.0);
}
