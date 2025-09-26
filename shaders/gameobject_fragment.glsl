#version 450 core

layout (location = 0) in vec3 O_color;
layout (location = 1) flat in uint O_tex_id;
layout (location = 2) in vec2 O_tex_coords;

layout (location = 0) out vec4 color;

void main() {
	color = vec4(O_color, 1.0);
}
