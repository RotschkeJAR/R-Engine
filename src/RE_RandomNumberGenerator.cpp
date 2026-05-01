#include "RE_Internal Header.hpp"

namespace RE {
	
	RandomNumberGenerator::RandomNumberGenerator() {
		seed_randomly();
	}
	RandomNumberGenerator::RandomNumberGenerator(const size_t seed) : rng(seed) {}
	RandomNumberGenerator::RandomNumberGenerator(const RandomNumberGenerator &rCopy) : rng(rCopy.rng) {}
	RandomNumberGenerator::~RandomNumberGenerator() {}

	void RandomNumberGenerator::seed(const size_t newSeed) {
		PRINT_DEBUG_CLASS("Seeding random number generator with ", std::hex, newSeed);
		rng.seed(newSeed);
	}

	size_t RandomNumberGenerator::seed_randomly() {
		PRINT_DEBUG_CLASS("Generating a random seed");
		size_t randomSeed;
		try {
			std::random_device rd;
			randomSeed = rd();
		} catch (...) {
			RE_ERROR("Failed seeding the random number generator due to the lack of a true random number generator. Using an undefined value instead");
		}
		this->seed(randomSeed);
		return randomSeed;
	}

	[[nodiscard]]
	double RandomNumberGenerator::random_normal() {
		return random<double>(1.0);
	}

	[[nodiscard]]
	bool RandomNumberGenerator::random_bool(const double f64Chance) {
		PRINT_DEBUG_CLASS("Rolling random number and checking if it's equal or lower than ", f64Chance);
		return random_normal() < f64Chance;
	}

}
