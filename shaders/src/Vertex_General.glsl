#version 450 core

layout (location = 0) in vec4 I_vertex_position;
layout (location = 1) in vec2 I_vertex_textureCoords;
layout (location = 2) in uint I_instance_index;

layout (location = 0) out vec4 O_color;
layout (location = 1) out vec2 O_textureCoords;
layout (location = 2) flat out uint O_textureId;


struct GameObject {
	float position[3];
	float rotation[3];
	float scale[3];
	float color[4];
	uint textureId;
};

struct GameObjectModelMatrix {
	mat4 modelMatrix;
};


layout(set = 0, binding = 0) readonly buffer GameObjectBuffer {
	GameObject data[];
} gameObjects;

layout(set = 0, binding = 1) readonly buffer ModelMatrixBuffer {
	GameObjectModelMatrix data[];
} modelMatrices;

layout(set = 1, binding = 0) uniform CameraMatrices {
	mat4 view;
	mat4 projection;
} cam;


void main() {
	// Optimization: Discard vertices by rendering them outside view
	if (gameObjects.data[I_instance_index].color[3] <= 0.0
			|| gameObjects.data[I_instance_index].scale[0] == 0.0
			|| gameObjects.data[I_instance_index].scale[1] == 0.0
			|| gameObjects.data[I_instance_index].scale[2] == 0.0) {
		gl_Position = vec4(-2.0, -2.0, -2.0, 1.0);
		return;
	}
	
	gl_Position = cam.projection * cam.view * modelMatrices.data[I_instance_index].modelMatrix * I_vertex_position;
	O_color = vec4(
			gameObjects.data[I_instance_index].color[0],
			gameObjects.data[I_instance_index].color[1],
			gameObjects.data[I_instance_index].color[2],
			gameObjects.data[I_instance_index].color[3]);
	O_textureCoords = I_vertex_textureCoords;
	O_textureId = gameObjects.data[I_instance_index].textureId;
}
