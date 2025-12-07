#version 450 core

layout(location = 0) in vec4 O_color;
layout(location = 1) in vec2 O_textureCoords;
layout(location = 2) flat in uint O_textureId;

layout(set = 1, binding = 0) uniform sampler2D U_textures[0x7FFF];

layout(location = 0) out vec4 color;

void main() {
	if (O_textureId < 0x8000)
		color = texture(U_textures[O_textureId], O_textureCoords) * O_color;
	else
		color = O_color;
}
