#version 450 core

layout (constant_id = 0) const uint shadowExtent = 4;
layout (constant_id = 1) const uint edgeWidth = 1;
layout (constant_id = 2) const uint barHeight = 30;
layout (constant_id = 3) const uint buttonWidth = 50;
layout (constant_id = 4) const uint buttonCount = 3;
layout (constant_id = 5) const uint buttonTextureSize = 8;
layout (constant_id = 6) const uint charTextureSize = 8;
layout (constant_id = 7) const uint charGap = 2;

layout (push_constant, std430) uniform PushConstants {
	uvec2 windowSize;
	uint windowFrameToRender;
	bool renderEdges;
};

layout (location = 0) in vec4 vertexInfo;
layout (location = 1) in vec2 uv;
layout (location = 2) flat in uint buttonIndex;
layout (location = 3) flat in uint characterIndex;

layout (set = 0, binding = 1) uniform sampler2DArray buttonTextureArray;

layout (set = 1, binding = 0) uniform sampler2DArray characterTextureArray;

layout (location = 0) out vec4 color;

void main() {
	if (buttonIndex != 0xFFFFFFFFu) {
		color = texture(buttonTextureArray, vec3(uv, float(buttonIndex))) * vertexInfo;
		if (color.w < 1.0)
			discard;
	} else if (characterIndex != 0xFFFFFFFFu) {
		if (gl_FragCoord.x >= windowSize.x - buttonWidth * buttonCount - (renderEdges ? (shadowExtent + edgeWidth) : 0) - 1)
			discard;
		color = texture(characterTextureArray, vec3(uv, float(characterIndex))) * vec4(0.0, 0.0, 0.0, 1.0);
		if (color.w < 1.0)
			discard;
	} else
		color = vertexInfo;
}
