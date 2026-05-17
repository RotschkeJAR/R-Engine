#version 450 core

struct GameObject {
	float position[3];
	float rotation[3];
	float scale[3];
	float color[4];
	uint textureId;
};

struct Depth {
	float depth;
	uint objectIndex;
};


layout (std430, set = 0, binding = 0) readonly buffer GameObjectBuffer {
	GameObject data[];
} gameObjects;

layout (std430, set = 0, binding = 1) buffer GameObjectModelMatrixBuffer {
	mat4 models[];
} modelMatrices;

layout (set = 1, binding = 0) uniform CameraMatrices {
	mat4 view;
	mat4 projection;
} cam;

layout (std430, set = 2, binding = 0) writeonly buffer SortableDepthBuffer {
	Depth data[];
} depths;


layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	modelMatrices.models[index] = mat4(
		gameObjects.data[index].scale[0],		0.0,									0.0,									0.0,
		0.0,									gameObjects.data[index].scale[1],		0.0,									0.0,
		0.0,									0.0,									gameObjects.data[index].scale[2],		0.0,
		gameObjects.data[index].position[0],	gameObjects.data[index].position[1],	gameObjects.data[index].position[2],	1.0
	);
	depths.data[index].depth = vec4(cam.projection * cam.view * modelMatrices.models[index] * vec4(0.0, 0.0, 0.0, 1.0)).z;
	depths.data[index].objectIndex = index;
}
