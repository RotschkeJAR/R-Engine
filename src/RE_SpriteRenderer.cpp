#include "RE_Internal.hpp"

namespace RE {
	
	SpriteRenderer::SpriteRenderer() {}
	SpriteRenderer::SpriteRenderer(const SpriteRenderer &rCopy) : color(rCopy.color) {}
	SpriteRenderer::~SpriteRenderer() {}

	void SpriteRenderer::copy_from(const SpriteRenderer &rCopy) {
		color = rCopy.color;
	}
	
	[[nodiscard]]
	bool SpriteRenderer::equals(const SpriteRenderer &rOther) const {
		return color == rOther.color;
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
