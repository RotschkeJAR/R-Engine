#ifndef __RE_RENDERER_SHADER_STRUCTS_H__
#define __RE_RENDERER_SHADER_STRUCTS_H__

namespace RE {

typedef uint32_t ShaderBool;
	constexpr ShaderBool SHR_TRUE = 1;
	constexpr ShaderBool SHR_FALSE = 0;

	struct WindowShaderData final {
		uint32_t a2u32Size[2];
		uint32_t u32WindowFrameToRender;
		ShaderBool b32RenderEdges;
	};

	struct WindowFrameUniformData final {
		uint32_t a2u32CursorPosition[2];
		uint32_t u32HoveredWindowAreaIndex;
		uint32_t au32TitleChars[256];
	};

	struct VertexData final {
		float a4f32Coords[4],
			a2f32TexCoords[2];
	};

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
