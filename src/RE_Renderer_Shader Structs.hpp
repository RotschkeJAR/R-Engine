#ifndef __RE_RENDERER_SHADER_STRUCTS_H__
#define __RE_RENDERER_SHADER_STRUCTS_H__

namespace RE {

	struct CameraShaderData final {
		float projectionMatrix[16];
		float viewMatrix[16];
	};

	struct GameObjectShaderData final {
		float position[3];
		float rotation[3];
		float scale[3];
		float color[4];
		uint32_t textureId;
	};
	
	struct GameObjectModelMatrixShaderData final {
		float modelMatrix[16];
	};
	
	struct DepthShaderData final {
		float depth;
		uint32_t objectIndex;
	};

}

#endif /* __RE_RENDERER_SHADER_STRUCTS_H__ */
