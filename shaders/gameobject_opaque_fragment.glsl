#version 450 core

layout(location = 0) in vec3 outColor;

layout(location = 0) out vec4 color;

void main() {
	color = vec4(outColor, 1.0);
}
