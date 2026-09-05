#ifndef __RE_RENDERER_SHADER_STRUCTS_H__
#define __RE_RENDERER_SHADER_STRUCTS_H__ 1

#include "RE_Renderer.hpp"

namespace RE {

	typedef uint32_t ShaderBool;
#define SHR_TRUE   1
#define SHR_FALSE  0

	struct WindowShaderData final {
		uint32_t a2u32Size[2];
		uint32_t u32WindowFrameToRender;
		ShaderBool b32RenderEdges;
	};

	struct WindowFrameUniformData final {
		uint32_t u32HoveredWindowAreaIndex;
		uint32_t au32TitleChars[MAX_CHARS_IN_WINDOW_TITLE];
	};

	struct VertexData final {
		float a4fCoords[4];
		float a2fTexCoords[2];
	};

	struct CameraShaderData final {
		float a16fViewMatrix[16];
		float a16fProjectionMatrix[16];
	};

	struct GameObjectShaderData final {
		float a3fPosition[3];
		int: 32;   // Padding
		float a3fRotation[3];
		int: 32;   // Padding
		float a3fScale[3];
		int: 32;   // Padding
		float a4fColor[4];
		uint32_t u32TextureId;
	};
	
	struct GameObjectModelMatrixShaderData final {
		float a16fModelMatrix[16];
	};

	struct GameObjectCountShaderData final {
		uint32_t u32Count;
	};
	
	struct DepthShaderData final {
		float fDepth;
		uint32_t u32ObjectIndex;
	};

}

#endif /* __RE_RENDERER_SHADER_STRUCTS_H__ */
