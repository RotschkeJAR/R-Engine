#version 450 core

layout(location = 0) in vec4 O_color;

layout(binding = 1) uniform sampler2D U_textures[0x7FFF];

layout(location = 0) out vec4 color;

void main() {
	color = O_color;
}
