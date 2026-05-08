#version 450 core
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) in vec4 O_color;
layout (location = 1) in vec2 O_textureCoords;
layout (location = 2) flat in uint O_textureId;

layout (set = 2, binding = 0) uniform sampler samplers[];
layout (set = 2, binding = 1) uniform texture2D textures[];

layout (location = 0) out vec4 color;

void main() {
	if (O_textureId != 0)
		color = texture(sampler2D(textures[nonuniformEXT(O_textureId) & 0xFFFF], samplers[(nonuniformEXT(O_textureId) >> 16) & 0xFFFF]), O_textureCoords) * O_color;
	else
		color = O_color;
}
