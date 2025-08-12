#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in float inTexId;
layout (location = 3) in vec2 inTexCoords;

layout (location = 0) out vec3 outColor;
layout (location = 1) out float outTexId;
layout (location = 2) out vec2 outTexCoords;

layout (binding = 0) uniform CameraUniformBuffer {
	mat4 view;
	mat4 projection;
} camUniform;

void main() {
	gl_Position = camUniform.projection * camUniform.view * vec4(inPosition, 1.0);
	gl_Position.z = (gl_Position.z + 1.0) * 0.5;
	outColor = inColor;
	outTexId = inTexId;
	outTexCoords = inTexCoords;
}
