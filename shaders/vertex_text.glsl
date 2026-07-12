#version 450 core

layout (location = 0) in uint charCode;

layout (location = 0) out vec2 uv;
layout (location = 1) flat out uint charIndex;

#define FIRST_CHAR_CODE      0x20

void main() {
	if (charCode < FIRST_CHAR_CODE) {
		// Discarding unrenderable characters
		gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}
	charIndex = (charCode - FIRST_CHAR_CODE) % 0x5F;
	vec4 position;
	switch (gl_VertexIndex) {
		case 0:
			position = vec4(0.0, 0.0, 0.0, 1.0);
			uv = vec2(0.0, 0.0);
			break;
		case 1:
			position = vec4(1.0, 0.0, 0.0, 1.0);
			uv = vec2(1.0, 0.0);
			break;
		case 2:
			position = vec4(0.0, 1.0, 0.0, 1.0);
			uv = vec2(0.0, 1.0);
			break;
		case 3:
			position = vec4(1.0, 1.0, 0.0, 1.0);
			uv = vec2(1.0, 1.0);
			break;
	}
	gl_Position = position;
}
