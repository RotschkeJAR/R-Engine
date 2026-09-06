#version 450 core

#extension GL_EXT_scalar_block_layout : require

struct GameObject {
	vec3 position;
	vec3 rotation;
	vec3 scale;
	vec4 color;
	uint textureId;
};

struct Depth {
	float depth;
	uint objectIndex;
};


layout (std430, set = 0, binding = 0) writeonly buffer SortableDepthBuffer {
	Depth data[];
} depths;

layout (std430, set = 1, binding = 0) buffer GameObjectBuffer {
	GameObject data[];
} gameObjects;

layout (std430, set = 1, binding = 1) buffer GameObjectModelMatrixBuffer {
	mat4 models[];
} modelMatrices;

layout (std430, set = 1, binding = 2) uniform GameObjectCountBuffer {
	uint count;
} gameObjectsCounts;

layout (set = 2, binding = 0) uniform CameraMatrices {
	mat4 view;
	mat4 projection;
} cam;


layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main() {
	if (gl_LocalInvocationIndex >= gameObjectsCounts.count)
		return;
	for (uint scaleCoordIndex = 0; scaleCoordIndex < 3; scaleCoordIndex++)
		if (gameObjects.data[gl_LocalInvocationIndex].scale[scaleCoordIndex] == 0.0) {
			gameObjects.data[gl_LocalInvocationIndex].color.w = -1.0;
			return;
		}
	modelMatrices.models[gl_LocalInvocationIndex] = mat4(
			gameObjects.data[gl_LocalInvocationIndex].scale.x,      0.0,                                                    0.0,                                                    0.0,
			0.0,                                                    gameObjects.data[gl_LocalInvocationIndex].scale.y,      0.0,                                                    0.0,
			0.0,                                                    0.0,                                                    gameObjects.data[gl_LocalInvocationIndex].scale.z,      0.0,
			gameObjects.data[gl_LocalInvocationIndex].position.x,   gameObjects.data[gl_LocalInvocationIndex].position.y,   gameObjects.data[gl_LocalInvocationIndex].position.z,   1.0);
	depths.data[gl_LocalInvocationIndex].depth = vec4(cam.projection * cam.view * modelMatrices.models[gl_LocalInvocationIndex] * vec4(0.0, 0.0, 0.0, 1.0)).z;
	depths.data[gl_LocalInvocationIndex].objectIndex = gl_LocalInvocationIndex;
}
