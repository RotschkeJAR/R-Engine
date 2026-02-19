#version 450 core

layout(location = 0) in vec3 I_vertex_position;
layout(location = 1) in vec2 I_vertex_textureCoords;

layout(location = 0) out vec4 O_color;
layout(location = 1) out vec2 O_textureCoords;
layout(location = 2) flat out uint O_textureId;


struct GameObject {
	mat4 model;
	vec4 color;
	uint textureId;
};

struct Depth {
	float depth;
	uint objectIndex;
};


layout(set = 0, binding = 0) readonly buffer GameObjectBuffer {
	GameObject data[];
} gameObjects;

layout(set = 0, binding = 1) readonly buffer DepthBuffer {
	Depth data[];
} depths;

layout(set = 0, binding = 2) uniform CameraMatrices {
	mat4 view;
	mat4 projection;
} cam;

void main() {
	const uint index = depths.data[gl_InstanceIndex].objectIndex;
	gl_Position = cam.projection * cam.view * gameObjects.data[index].model * vec4(I_vertex_position, 1.0);
	O_color = gameObjects.data[index].color;
	O_textureCoords = I_vertex_textureCoords;
	O_textureId = gameObjects.data[index].textureId;
}
