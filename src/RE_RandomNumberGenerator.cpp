#include "RE_Internal.hpp"

namespace RE {
	
	RandomNumberGenerator::RandomNumberGenerator() {
		seed_randomly();
	}
	RandomNumberGenerator::RandomNumberGenerator(const size_t sSeed) : std_rng(sSeed) {}
	RandomNumberGenerator::RandomNumberGenerator(const RandomNumberGenerator &rCopy) : std_rng(rCopy.std_rng) {}
	RandomNumberGenerator::~RandomNumberGenerator() {}

	void RandomNumberGenerator::seed(const size_t sNewSeed) {
		PRINT_DEBUG_CLASS("Seeding random number generator with ", std::hex, sNewSeed);
		std_rng.seed(sNewSeed);
	}

	size_t RandomNumberGenerator::seed_randomly() {
		PRINT_DEBUG_CLASS("Generating a random seed");
		size_t randomSeed;
		try {
			std::random_device rd;
			randomSeed = rd();
		} catch (...) {
			RE_ERROR("Failed seeding the random number generator due to the lack of a true random number generator. Using time instead");
			randomSeed = std::time(0);
		}
		this->seed(randomSeed);
		return randomSeed;
	}

	[[nodiscard]]
	double RandomNumberGenerator::random_normal() {
		return random<double>(1.0);
	}

	[[nodiscard]]
	bool RandomNumberGenerator::random_bool(const double dChance) {
		PRINT_DEBUG_CLASS("Rolling random number and checking if it's equal or lower than ", dChance);
		return random_normal() < dChance;
	}

}
