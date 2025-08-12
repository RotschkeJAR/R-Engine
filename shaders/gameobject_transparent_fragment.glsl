#version 450 core

layout (location = 0) in vec4 outColor;
layout (location = 1) in float outTexId;
layout (location = 2) in vec2 outTexCoords;

layout (location = 0) out vec4 color;

layout (binding = 1) uniform sampler2D textures[32];

void main() {
	if (outTexId == 0.0)
		color = outColor;
	else
		color = texture(textures[int(outTexId) - 1], outTexCoords) * color;
}
