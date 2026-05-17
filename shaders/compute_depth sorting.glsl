#version 450 core

struct Depth {
	float depth;
	uint objectIndex;
};


layout (std430, set = 2, binding = 0) buffer DepthBuffer {
	Depth data[];
} depths;


layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main() {
	
}
