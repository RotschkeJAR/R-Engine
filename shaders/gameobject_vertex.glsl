#version 450 core

layout(location = 0) in vec3 I_vertex_position;
layout(location = 1) in vec2 I_vertex_textureCoords;
layout(location = 2) in mat4 I_instance_modelMatrix;
layout(location = 6) in vec4 I_instance_color;
layout(location = 7) in uint I_instance_textureId;

layout(location = 0) out vec4 O_color;
layout(location = 1) out vec2 O_textureCoords;
layout(location = 2) flat out uint O_textureId;

layout(binding = 0) uniform CameraMatrices {
	mat4 view;
	mat4 projection;
} cam;

void main() {
	gl_Position = cam.projection * cam.view * I_instance_modelMatrix * vec4(I_vertex_position, 1.0);
	O_color = I_instance_color;
	O_textureCoords = I_vertex_textureCoords;
	O_textureId = I_instance_textureId;
}
