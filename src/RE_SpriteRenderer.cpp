#include "RE_Internal.hpp"

namespace RE {
	
	SpriteRenderer::SpriteRenderer() : textureCoordinates(1.0f, 1.0f) {}
	SpriteRenderer::SpriteRenderer(const SpriteRenderer &rCopy) : color(rCopy.color),
			textureOffset(rCopy.textureOffset),
			textureCoordinates(rCopy.textureCoordinates) {}
	SpriteRenderer::~SpriteRenderer() {}

	void SpriteRenderer::copy_from(const SpriteRenderer &rCopy) {
		color = rCopy.color;
		textureOffset = rCopy.textureOffset;
		textureCoordinates = rCopy.textureCoordinates;
	}
	
	[[nodiscard]]
	bool SpriteRenderer::equals(const SpriteRenderer &rOther) const {
		return color == rOther.color
				and textureOffset == rOther.textureOffset
				and textureCoordinates == rOther.textureCoordinates;
	}

	void SpriteRenderer::operator =(const SpriteRenderer &rCopy) {
		copy_from(rCopy);
	}

	[[nodiscard]]
	bool SpriteRenderer::operator ==(const SpriteRenderer &rOther) const {
		return equals(rOther);
	}

	[[nodiscard]]
	bool SpriteRenderer::operator !=(const SpriteRenderer &rOther) const {
		return not equals(rOther);
	}

}
