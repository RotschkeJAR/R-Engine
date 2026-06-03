#version 450 core

layout (push_constant, std430) uniform PushConstants {
	uvec2 windowSize;
	bool renderEdges;
	bool renderBar;
};

layout (constant_id = 0) const uint shadowExtent = 4;
layout (constant_id = 1) const uint borderWidth = 1;
layout (constant_id = 2) const uint barHeight = 30;
layout (constant_id = 3) const uint buttonWidth = 50;

layout (set = 0, binding = 0) uniform Cursor {
	uvec2 position;
};

layout (location = 0) out vec4 color;

void main() {
	if (gl_FragCoord.x < shadowExtent
			|| gl_FragCoord.x >= windowSize.x - shadowExtent
			|| gl_FragCoord.y < shadowExtent
			|| gl_FragCoord.y >= windowSize.y - shadowExtent)
		color = vec4(0.0, 0.0, 0.0, 0.0);
	else if ((gl_FragCoord.x >= shadowExtent && gl_FragCoord.x < shadowExtent + borderWidth)
			|| (gl_FragCoord.x >= windowSize.x - shadowExtent - borderWidth && gl_FragCoord.x < windowSize.x - shadowExtent)
			|| (gl_FragCoord.y >= shadowExtent && gl_FragCoord.y < shadowExtent + borderWidth + barHeight)
			|| (gl_FragCoord.y >= windowSize.y - shadowExtent - borderWidth && gl_FragCoord.y < windowSize.y - shadowExtent))
		color = vec4(1.0, 1.0, 1.0, 1.0);
	else
		discard;
}
