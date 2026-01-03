struct RawRenderContent {
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

layout(std140, set = 0, binding = 0) readonly buffer RawRenderBuffer {
	RawRenderContent rawContent[];
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectCount {
	uint objectCount;
};

layout(std140, set = 1, binding = 1) buffer RenderBuffer {
	RenderContent renderContent[];
};

layout(std140, set = 1, binding = 2) buffer DepthBuffer {
	DepthContent depthContent[];
};

void main() {
	
}
