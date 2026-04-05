#version 450 core

struct Depth {
	float depth;
	uint objectIndex;
};

layout(std430, set = 1, binding = 1) buffer DepthBuffer {
	Depth data[];
} depths;

layout (local_size_x = 500, local_size_y = 1, local_size_z = 1) in;

void main() {
	
}
