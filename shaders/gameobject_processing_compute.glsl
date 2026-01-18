#version 450 core

struct GameObject {
	vec3 position;
	vec3 rotation;
	vec3 scale;	
};

struct RenderContent {
	mat4 position;
};

struct DepthContent {
	float depth;
	uint objectIndex;
};

layout(std140, set = 0, binding = 0) readonly buffer RawGameObjectBuffer {
	GameObject gameObjects[];
};

layout(std140, set = 1, binding = 0) buffer RenderBuffer {
	RenderContent renderContent[];
};

layout(std140, set = 1, binding = 1) buffer DepthBuffer {
	DepthContent depthContent[];
};

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main() {
	
}
