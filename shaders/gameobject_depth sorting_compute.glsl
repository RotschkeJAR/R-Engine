#version 450 core

struct GameObject {
	mat4 position;
	vec4 color;
	uint textureId;
};

struct Depth {
	float depth;
	uint objectIndex;
};

layout(std430, set = 1, binding = 0) buffer GameObjectBuffer {
	GameObject data[];
} gameObjects;

layout(std430, set = 1, binding = 1) buffer DepthBuffer {
	Depth data[];
} depths;

layout (local_size_x = 1000, local_size_y = 1, local_size_z = 1) in;

void main() {
	
}
