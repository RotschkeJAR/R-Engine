#version 450 core

layout (location = 0) in vec4 outColor;
layout (location = 1) in float outTexId;
layout (location = 2) in vec2 outTexCoords;

layout (location = 0) out vec4 color;

layout (binding = 1) uniform sampler2D textures[0x7FFF];

void main() {
	if (outTexId < -0.5)
		color = outColor;
	else
		color = texture(textures[int(outTexId)], outTexCoords) * color;
}
