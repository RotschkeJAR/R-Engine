#version 450 core

layout (constant_id = 0) const int SHADOW_EXTENT = 4;
layout (constant_id = 1) const int EDGE_WIDTH = 1;
layout (constant_id = 2) const int BAR_HEIGHT = 30;
layout (constant_id = 3) const int BUTTON_WIDTH = 50;
layout (constant_id = 4) const int BUTTON_COUNT = 3;
layout (constant_id = 5) const int BUTTON_TEXTURE_SIZE = 8;
layout (constant_id = 6) const int CHAR_TEXTURE_SIZE = 8;
layout (constant_id = 7) const int CHAR_GAP = 2;
layout (constant_id = 8) const int CHAR_SCALE = 1;
layout (constant_id = 9) const int WINDOW_AREA_BUTTON_CLOSE_INDEX = 10;
layout (constant_id = 10) const int MAX_CHARS_TITLE = 256;
layout (constant_id = 11) const uint DONT_RENDER_BUTTON = ~0U;
layout (constant_id = 12) const uint DONT_RENDER_CHARACTER = ~0U;

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
	if (buttonIndex != DONT_RENDER_BUTTON) {
		color = texture(buttonTextureArray, vec3(uv, float(buttonIndex))) * vertexInfo;
		if (color.w < 1.0)
			discard;
	} else if (characterIndex != DONT_RENDER_CHARACTER) {
		if (gl_FragCoord.x >= windowSize.x - BUTTON_WIDTH * BUTTON_COUNT - (renderEdges ? (SHADOW_EXTENT + EDGE_WIDTH) : 0) - 1)
			discard;
		color = texture(characterTextureArray, vec3(uv, float(characterIndex))) * vec4(0.0, 0.0, 0.0, 1.0);
		if (color.w < 1.0)
			discard;
	} else
		color = vertexInfo;
}
