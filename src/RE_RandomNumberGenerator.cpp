#include "RE.hpp"

namespace RE {
	
	RandomNumberGenerator::RandomNumberGenerator() : rng(std::random_device{}()) {}
	RandomNumberGenerator::RandomNumberGenerator(REuint seed) : rng(seed) {}
	RandomNumberGenerator::~RandomNumberGenerator() {}

	REuint RandomNumberGenerator::random() {
		return random(std::numeric_limits<REuint>::max());
	}

	REuint RandomNumberGenerator::random(REuint max) {
		return random(0U, max);
	}

	REuint RandomNumberGenerator::random(REuint min, REuint max) {
		std::uniform_int_distribution<REuint> range(min, max - 1U);
		return range(rng);
	}

	bool RandomNumberGenerator::randomBool() {
		return static_cast<bool>(random() & 1U);
	}

}
