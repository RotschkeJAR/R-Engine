#ifndef __RE_RENDERER_SHADER_STRUCTS_H__
#define __RE_RENDERER_SHADER_STRUCTS_H__

namespace RE {

	struct alignas(16) CameraShaderData final {
		float projectionMatrix[16];
		float viewMatrix[16];
	};

	struct alignas(16) RawGameObjectShaderData final {
		float position[3];
		float rotation[3];
		float scale[3];
		float color[4];
		uint32_t textureId;
	};
	
	struct alignas(16) GameObjectShaderData final {
		float modelMatrix[16];
		float color[4];
		uint32_t textureId;
	};
	
	struct alignas(16) DepthShaderData final {
		float depth;
		uint32_t objectIndex;
	};

}

#endif /* __RE_RENDERER_SHADER_STRUCTS_H__ */
