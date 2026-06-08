#version 450 core

layout (push_constant, std430) uniform PushConstants {
	uvec2 windowSize;
	uint windowFrameToRender;
	bool renderEdges;
};

const uint shadowsToRender = 0;
const uint barToRender = 1;
const uint buttonsToRender = 2;

layout (constant_id = 0) const uint shadowExtent = 4;
layout (constant_id = 1) const uint edgeWidth = 1;
layout (constant_id = 2) const uint barHeight = 30;
layout (constant_id = 3) const uint buttonWidth = 50;

layout (location = 0) out vec4 vertexColor;

vec2 pixel_to_ndc(uvec2 pixel) {
	return pixel / vec2(windowSize) * 2.0 - 1.0;
}

void main() {
	switch (windowFrameToRender) {
		case shadowsToRender:
			switch (gl_VertexIndex) {
				case 0:
				case 8:
					gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
					vertexColor = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 1:
				case 9:
					gl_Position = vec4(pixel_to_ndc(uvec2(shadowExtent - 1, shadowExtent - 1)), 0.0, 1.0);
					vertexColor = vec4(0.0, 0.0, 0.0, 1.0);
					break;
				case 2:
					gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
					vertexColor = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 3:
					gl_Position = vec4(pixel_to_ndc(uvec2(shadowExtent - 1, windowSize.y - shadowExtent)), 0.0, 1.0);
					vertexColor = vec4(0.0, 0.0, 0.0, 1.0);
					break;
				case 4:
					gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
					vertexColor = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 5:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - shadowExtent, windowSize.y - shadowExtent)), 0.0, 1.0);
					vertexColor = vec4(0.0, 0.0, 0.0, 1.0);
					break;
				case 6:
					gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
					vertexColor = vec4(0.0, 0.0, 0.0, 0.0);
					break;
				case 7:
					gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - shadowExtent, shadowExtent - 1)), 0.0, 1.0);
					vertexColor = vec4(0.0, 0.0, 0.0, 1.0);
					break;
			}
			break;

		// Edges
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
			vertexColor = vec4(0.8, 0.8, 0.8, 1.0);
			break;
			
		// Close button
		case buttonsToRender:
			{
				const float offsetFromRightmost = renderEdges ? (shadowExtent + 1 + edgeWidth) : 0,
					offsetFromTop = renderEdges ? (shadowExtent + edgeWidth) : 0;
				switch (gl_VertexIndex) {
					case 0:
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - buttonWidth * gl_InstanceIndex - buttonWidth - offsetFromRightmost, offsetFromTop)), 0.0, 1.0);
						break;
					case 1:
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - buttonWidth * gl_InstanceIndex - offsetFromRightmost, offsetFromTop)), 0.0, 1.0);
						break;
					case 2:
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - buttonWidth * gl_InstanceIndex - buttonWidth - offsetFromRightmost, offsetFromTop + barHeight)), 0.0, 1.0);
						break;
					case 3:
						gl_Position = vec4(pixel_to_ndc(uvec2(windowSize.x - buttonWidth * gl_InstanceIndex - offsetFromRightmost, offsetFromTop + barHeight)), 0.0, 1.0);
						break;
				}
			}
			switch (gl_InstanceIndex) {
				case 0:
					vertexColor = vec4(0.9, 0.0, 0.0, 1.0);
					break;
				case 1:
					vertexColor = vec4(0.05, 0.05, 1.0, 1.0);
					break;
				case 2:
					vertexColor = vec4(0.05, 0.05, 1.0, 1.0);
					break;
			}
			break;
	}
}
