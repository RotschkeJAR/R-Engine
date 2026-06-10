#version 450 core

layout (location = 0) in vec4 vertexInfo;
layout (location = 1) in vec2 uv;
layout (location = 2) flat in uint buttonIndex;

layout (set = 0, binding = 1) uniform sampler2DArray buttonTextureArray;

layout (location = 0) out vec4 color;

void main() {
	if (buttonIndex == 0xFFFFFFFF)
		color = vertexInfo;
	else {
		color = texture(buttonTextureArray, vec3(uv, float(buttonIndex))) * vertexInfo;
		if (color.w == 0.0)
			discard;
	}
}
