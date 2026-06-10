#version 450 core

layout (location = 0) in uint charCode;

layout (location = 0) out vec2 uv;
layout (location = 1) flat out uint charIndex;

void main() {
	charIndex = charCode;
	switch (gl_VertexIndex) {
		case 0:
			gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
			uv = vec2(0.0, 0.0);
			break;
		case 1:
			gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
			uv = vec2(1.0, 0.0);
			break;
		case 2:
			gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
			uv = vec2(0.0, 1.0);
			break;
		case 3:
			gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
			uv = vec2(1.0, 1.0);
			break;
	}
}
