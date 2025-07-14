#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform CameraUniformBuffer {
	mat4 view;
	mat4 projection;
} camUniform;

void main() {
	gl_Position = camUniform.projection * camUniform.view * vec4(inPosition, 1.0);
	gl_Position.z = (gl_Position.z + 1.0) * 0.5;
	gl_Position.y *= -1.0;
	outColor = inColor;
}
