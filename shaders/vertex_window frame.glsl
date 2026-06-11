#version 450 core

#extension GL_EXT_scalar_block_layout : enable

const uint shadowsToRender = 0;
const uint barToRender = 1;
const uint buttonsToRender = 2;
const uint titleToRender = 3;

layout (constant_id = 0) const uint shadowExtent = 4;
layout (constant_id = 1) const uint edgeWidth = 1;
layout (constant_id = 2) const uint barHeight = 30;
layout (constant_id = 3) const uint buttonWidth = 50;
layout (constant_id = 4) const uint buttonCount = 3;
layout (constant_id = 5) const uint buttonTextureSize = 8;
layout (constant_id = 6) const uint charTextureSize = 8;
layout (constant_id = 7) const uint charGap = 2;
layout (constant_id = 8) const uint charScale = 1;

layout (push_constant, std430) uniform PushConstants {
	uvec2 windowSize;
	uint windowFrameToRender;
	bool renderEdges;
};

layout (std430, set = 0, binding = 0) uniform Metadata {
	uvec2 cursorPosition;
	uint titleChars[256];
} metadata;

layout (location = 0) out vec4 vertexInfo;
layout (location = 1) out vec2 uv;
layout (location = 2) flat out uint buttonIndex;
layout (location = 3) flat out uint characterIndex;

vec2 pixel_to_ndc(uvec2 pixel) {
	return pixel / vec2(windowSize) * 2.0 - 1.0;
}

void main() {
	buttonIndex = 0xFFFFFFFF;
	characterIndex = 0xFFFFFFFF;
	switch (windowFrameToRender) {
		case shadowsToRender:
			switch (gl_VertexIndex) {
				case 0:
				case 8:
					gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 1:
				case 9:
					gl_Position = vec4(pixel_to_ndc(uvec2(shadowExtent - 1, shadowExtent - 1)), 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 1.0);
					break;
				case 2:
					gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 3:
					gl_Position = vec4(pixel_to_ndc(uvec2(shadowExtent - 1, windowSize.y - shadowExtent)), 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 1.0);
					break;
				case 4:
					gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 5:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - shadowExtent, windowSize.y - shadowExtent)), 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 1.0);
					break;
				case 6:
					gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 7:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - shadowExtent, shadowExtent - 1)), 0.0, 1.0);
					vertexInfo = vec4(0.0, 0.0, 0.0, 1.0);
					break;
			}
			break;

		case barToRender:
			switch (gl_VertexIndex) {
				case 0:
				case 8:
					if (renderEdges)
						gl_Position = vec4(pixel_to_ndc(uvec2(shadowExtent, shadowExtent)), 0.0, 1.0);
					else
						gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
					break;
				case 1:
				case 9:
					if (renderEdges)
						gl_Position = vec4(pixel_to_ndc(uvec2(shadowExtent + edgeWidth, shadowExtent + barHeight + edgeWidth)), 0.0, 1.0);
					else
						gl_Position = vec4(pixel_to_ndc(uvec2(0, barHeight)), 0.0, 1.0);
					break;
				case 2:
					if (renderEdges)
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - shadowExtent - 1, shadowExtent)), 0.0, 1.0);
					else
						gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
					break;
				case 3:
					if (renderEdges)
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - shadowExtent - 1 - edgeWidth, shadowExtent + barHeight + edgeWidth)), 0.0, 1.0);
					else
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x, barHeight)), 0.0, 1.0);
					break;
				case 4:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - shadowExtent - 1, windowSize.y - shadowExtent - 1)), 0.0, 1.0);
					break;
				case 5:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - shadowExtent - 1 - edgeWidth, windowSize.y - shadowExtent - 1 - edgeWidth)), 0.0, 1.0);
					break;
				case 6:
					gl_Position = vec4(pixel_to_ndc(uvec2(shadowExtent, windowSize.y - shadowExtent - 1)), 0.0, 1.0);
					break;
				case 7:
					gl_Position = vec4(pixel_to_ndc(uvec2(shadowExtent + edgeWidth, windowSize.y - shadowExtent - 1 - edgeWidth)), 0.0, 1.0);
					break;
			}
			vertexInfo = vec4(0.8, 0.8, 0.8, 1.0);
			break;
			
		case buttonsToRender:
			{
				buttonIndex = gl_InstanceIndex % buttonCount;
				uint offsetFromRight = windowSize.x - buttonWidth * buttonIndex - (renderEdges ? (shadowExtent + 1 + edgeWidth) : 0),
					offsetFromTop = renderEdges ? (shadowExtent + edgeWidth) : 0;
				const bool hovered = metadata.cursorPosition.x > offsetFromRight - buttonWidth
						&& metadata.cursorPosition.x <= offsetFromRight
						&& metadata.cursorPosition.y > offsetFromTop
						&& metadata.cursorPosition.y <= offsetFromTop + barHeight;
				if (gl_InstanceIndex < buttonCount) {
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
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight - buttonWidth, offsetFromTop)), 0.0, 1.0);
							break;
						case 2:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight, offsetFromTop + barHeight)), 0.0, 1.0);
							break;
						case 3:
							gl_Position = vec4(pixel_to_ndc(uvec2(offsetFromRight - buttonWidth, offsetFromTop + barHeight)), 0.0, 1.0);
							break;
					}
					buttonIndex = 0xFFFFFFFF;
				} else {
					const uint computedTextureSize = buttonTextureSize;
					vertexInfo = hovered ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
					offsetFromRight -= (buttonWidth - computedTextureSize) / 2;
					offsetFromTop += (barHeight - computedTextureSize) / 2;
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

		case titleToRender:
			const uint computedCharTextureSize = charTextureSize * charScale;
			const uint offsetFromLeft = (barHeight - computedCharTextureSize) / 2 + gl_InstanceIndex * (computedCharTextureSize + charGap) + (renderEdges ? (shadowExtent + edgeWidth) : 0),
				offsetFromTop = (barHeight - computedCharTextureSize) / 2 + (renderEdges ? (shadowExtent + edgeWidth) : 0);
			characterIndex = min(metadata.titleChars[gl_InstanceIndex], 256);
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
