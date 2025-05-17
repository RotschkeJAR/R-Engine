#include "RE_Internal Header.hpp"

namespace RE {
	
	RandomNumberGenerator::RandomNumberGenerator() {
		try {
			std::random_device rd;
			rng.seed(rd());
		} catch (...) {
			RE_ERROR("Failed seeding the random number generator due to the lack of a true random number generator. Using time instead");
			rng.seed(time(nullptr));
		}
	}
	RandomNumberGenerator::RandomNumberGenerator(uint32_t u32Seed) : rng(u32Seed) {}
	RandomNumberGenerator::~RandomNumberGenerator() {}

	void RandomNumberGenerator::set_seed(uint32_t newSeed) {
		rng.seed(newSeed);
	}

	bool RandomNumberGenerator::random_bool() {
		return static_cast<bool>(random<uint16_t>() & 1U);
	}

	bool RandomNumberGenerator::random_bool(double dChance) {
		return random_percentage() <= dChance;
	}

	double RandomNumberGenerator::random_percentage() {
		return static_cast<double>(random<uint32_t>()) / std::numeric_limits<uint32_t>::max();
	}

}
