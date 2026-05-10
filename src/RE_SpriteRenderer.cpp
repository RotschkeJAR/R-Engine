#include "RE_Internal Header.hpp"

namespace RE {
	
	SpriteRenderer::SpriteRenderer() : color(), textureOffset(), textureCoordinates(1.0f, 1.0f), hMesh(nullptr) {}
	SpriteRenderer::SpriteRenderer(const SpriteRenderer &rCopy) : color(rCopy.color), textureOffset(rCopy.textureOffset), textureCoordinates(rCopy.textureCoordinates), hMesh(rCopy.hMesh) {}
	SpriteRenderer::~SpriteRenderer() {}

}
