#version 450 core

struct RawGameObject {
	float position[3];
	float rotation[3];
	float scale[3];
	float color[4];
	uint textureId;
};

struct GameObject {
	mat4 model;
	vec4 color;
	uint textureId;
};

struct Depth {
	float depth;
	uint objectIndex;
};


layout(std430, set = 0, binding = 0) readonly buffer RawGameObjectBuffer {
	RawGameObject data[];
} rawGameObjects;

layout(std430, set = 1, binding = 0) buffer GameObjectBuffer {
	GameObject data[];
} gameObjects;

layout(std430, set = 1, binding = 1) writeonly buffer DepthBuffer {
	Depth data[];
} depths;

layout(set = 1, binding = 2) uniform CameraMatrices {
	mat4 view;
	mat4 projection;
} cam;


layout (local_size_x = 1000, local_size_y = 1, local_size_z = 1) in;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	gameObjects.data[index].model = mat4(
		rawGameObjects.data[index].scale[0],	0.0,									0.0,									0.0,
		0.0,									rawGameObjects.data[index].scale[1],	0.0,									0.0,
		0.0,									0.0,									rawGameObjects.data[index].scale[2],	0.0,
		rawGameObjects.data[index].position[0],	rawGameObjects.data[index].position[1],	rawGameObjects.data[index].position[2],	1.0
	);
	for (uint colorChannel = 0; colorChannel < 3; colorChannel++)
		gameObjects.data[index].color[colorChannel] = rawGameObjects.data[index].color[colorChannel];
	gameObjects.data[index].textureId = rawGameObjects.data[index].textureId;
	
	depths.data[index].objectIndex = index;
	depths.data[index].depth = vec4(cam.projection * cam.view * gameObjects.data[index].model * vec4(0.0, 0.0, 0.0, 1.0)).z;
}
