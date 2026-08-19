#version 450 core

#extension GL_EXT_scalar_block_layout : require

#define SHADOWS_TO_RENDER  0
#define BAR_TO_RENDER      1
#define BUTTONS_TO_RENDER  2
#define TITLE_TO_RENDER    3

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
layout (constant_id = 11) const uint DONT_RENDER_BUTTON = 0xFFFFFFFFu;
layout (constant_id = 12) const uint DONT_RENDER_CHARACTER = 0xFFFFFFFFu;

layout (push_constant, std430) uniform PushConstants {
	uvec2 windowSize;
	uint windowFrameToRender;
	bool renderEdges;
};

layout (std430, set = 0, binding = 0) uniform Metadata {
	uint hoveredWindowAreaIndex;
	uint titleChars[MAX_CHARS_TITLE];
} metadata;

layout (location = 0) out vec4 vertexInfo;
layout (location = 1) out vec2 uv;
layout (location = 2) flat out uint buttonIndex;
layout (location = 3) flat out uint characterIndex;

vec2 pixel_to_ndc(uvec2 pixel) {
	return pixel / vec2(windowSize) * 2.0 - 1.0;
}

void main() {
	buttonIndex = DONT_RENDER_BUTTON;
	characterIndex = DONT_RENDER_CHARACTER;
	switch (windowFrameToRender) {
		case SHADOWS_TO_RENDER:
			switch (gl_VertexIndex) {
				case 0:
				case 8:
					gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 1:
				case 9:
					gl_Position = vec4(pixel_to_ndc(uvec2(SHADOW_EXTENT - 1, SHADOW_EXTENT - 1)), 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 1.0);
					break;
				case 2:
					gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 3:
					gl_Position = vec4(pixel_to_ndc(uvec2(SHADOW_EXTENT - 1, windowSize.y - SHADOW_EXTENT)), 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 1.0);
					break;
				case 4:
					gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 5:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - SHADOW_EXTENT, windowSize.y - SHADOW_EXTENT)), 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 1.0);
					break;
				case 6:
					gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 7:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - SHADOW_EXTENT, SHADOW_EXTENT - 1)), 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 1.0);
					break;
			}
			break;

		case BAR_TO_RENDER:
			switch (gl_VertexIndex) {
				case 0:
				case 8:
					if (renderEdges)
						gl_Position = vec4(pixel_to_ndc(uvec2(SHADOW_EXTENT, SHADOW_EXTENT)), 0.0, 1.0);
					else
						gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
					break;
				case 1:
				case 9:
					if (renderEdges)
						gl_Position = vec4(pixel_to_ndc(uvec2(SHADOW_EXTENT + EDGE_WIDTH, SHADOW_EXTENT + BAR_HEIGHT + EDGE_WIDTH)), 0.0, 1.0);
					else
						gl_Position = vec4(pixel_to_ndc(uvec2(0, BAR_HEIGHT)), 0.0, 1.0);
					break;
				case 2:
					if (renderEdges)
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - SHADOW_EXTENT - 1, SHADOW_EXTENT)), 0.0, 1.0);
					else
						gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
					break;
				case 3:
					if (renderEdges)
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - SHADOW_EXTENT - 1 - EDGE_WIDTH, SHADOW_EXTENT + BAR_HEIGHT + EDGE_WIDTH)), 0.0, 1.0);
					else
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x, BAR_HEIGHT)), 0.0, 1.0);
					break;
				case 4:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - SHADOW_EXTENT - 1, windowSize.y - SHADOW_EXTENT - 1)), 0.0, 1.0);
					break;
				case 5:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - SHADOW_EXTENT - 1 - EDGE_WIDTH, windowSize.y - SHADOW_EXTENT - 1 - EDGE_WIDTH)), 0.0, 1.0);
					break;
				case 6:
					gl_Position = vec4(pixel_to_ndc(uvec2(SHADOW_EXTENT, windowSize.y - SHADOW_EXTENT - 1)), 0.0, 1.0);
					break;
				case 7:
					gl_Position = vec4(pixel_to_ndc(uvec2(SHADOW_EXTENT + EDGE_WIDTH, windowSize.y - SHADOW_EXTENT - 1 - EDGE_WIDTH)), 0.0, 1.0);
					break;
			}
			vertexInfo = vec4(0.8, 0.8, 0.8, 1.0);
			break;
			
		case BUTTONS_TO_RENDER:
			{
				buttonIndex = gl_InstanceIndex % BUTTON_COUNT;
				uint offsetFromRight = windowSize.x - BUTTON_WIDTH * buttonIndex - (renderEdges ? (SHADOW_EXTENT + 1 + EDGE_WIDTH) : 0),
					offsetFromTop = renderEdges ? (SHADOW_EXTENT + EDGE_WIDTH) : 0;
				const bool hovered = metadata.hoveredWindowAreaIndex - WINDOW_AREA_BUTTON_CLOSE_INDEX == buttonIndex;
				if (gl_InstanceIndex < BUTTON_COUNT) {
					if (!hovered) {
						gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
						return;
					}
					switch (buttonIndex) {
						case 0:
							vertexInfo = vec4(0.9, 0.0, 0.0, 1.0);
							break;
						case 1:
							vertexInfo = vec4(0.05, 0.05, 0.9, 1.0);
							break;
						case 2:
							vertexInfo = vec4(0.05, 0.05, 0.9, 1.0);
							break;
					}
					switch (gl_VertexIndex) {
						case 0:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight, offsetFromTop)), 0.0, 1.0);
							break;
						case 1:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight - BUTTON_WIDTH, offsetFromTop)), 0.0, 1.0);
							break;
						case 2:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight, offsetFromTop + BAR_HEIGHT)), 0.0, 1.0);
							break;
						case 3:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight - BUTTON_WIDTH, offsetFromTop + BAR_HEIGHT)), 0.0, 1.0);
							break;
					}
					buttonIndex = 0xFFFFFFFFu;
				} else {
					const uint computedTextureSize = BUTTON_TEXTURE_SIZE;
					vertexInfo = hovered ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
					offsetFromRight -= (BUTTON_WIDTH - computedTextureSize) / 2;
					offsetFromTop += (BAR_HEIGHT - computedTextureSize) / 2;
					switch (gl_VertexIndex) {
						case 0:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight, offsetFromTop)), 0.0, 1.0);
							uv = vec2(0.0, 0.0);
							break;
						case 1:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight - computedTextureSize, offsetFromTop)), 0.0, 1.0);
							uv = vec2(1.0, 0.0);
							break;
						case 2:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight, offsetFromTop + computedTextureSize)), 0.0, 1.0);
							uv = vec2(0.0, 1.0);
							break;
						case 3:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight - computedTextureSize, offsetFromTop + computedTextureSize)), 0.0, 1.0);
							uv = vec2(1.0, 1.0);
							break;
					}
				}
			}
			break;

		case TITLE_TO_RENDER:
			const uint computedCharTextureSize = CHAR_TEXTURE_SIZE * CHAR_SCALE;
			const uint offsetFromLeft = (BAR_HEIGHT - computedCharTextureSize) / 2 + gl_InstanceIndex * (computedCharTextureSize + CHAR_GAP) + (renderEdges ? (SHADOW_EXTENT + EDGE_WIDTH) : 0),
				offsetFromTop = (BAR_HEIGHT - computedCharTextureSize) / 2 + (renderEdges ? (SHADOW_EXTENT + EDGE_WIDTH) : 0);
			characterIndex = (metadata.titleChars[gl_InstanceIndex] - 0x20) % 0x5F;
			switch (gl_VertexIndex) {
				case 0:
					gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromLeft, offsetFromTop)), 0.0, 1.0);
					uv = vec2(0.0, 0.0);
					break;
				case 1:
					gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromLeft + computedCharTextureSize, offsetFromTop)), 0.0, 1.0);
					uv = vec2(1.0, 0.0);
					break;
				case 2:
					gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromLeft, offsetFromTop + computedCharTextureSize)), 0.0, 1.0);
					uv = vec2(0.0, 1.0);
					break;
				case 3:
					gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromLeft + computedCharTextureSize, offsetFromTop + computedCharTextureSize)), 0.0, 1.0);
					uv = vec2(1.0, 1.0);
					break;
			}
			break;
	}
}
