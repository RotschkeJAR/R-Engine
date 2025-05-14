#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;

layout (binding = 0) uniform mat4 uniCamPos;

layout (location = 0) out vec4 outColor;

void main() {
	gl_Position = uniCamPos * vec4(inPosition, 1.0);
	outColor = inColor;
}
