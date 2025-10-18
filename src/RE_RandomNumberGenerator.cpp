#include "RE_Internal Header.hpp"

namespace RE {
	
	RandomNumberGenerator::RandomNumberGenerator() {
		seed_randomly();
	}
	RandomNumberGenerator::RandomNumberGenerator(const size_t seed) : rng(seed) {}
	RandomNumberGenerator::~RandomNumberGenerator() {}

	void RandomNumberGenerator::seed(const size_t newSeed) {
		PRINT_DEBUG_CLASS("Seeding random number generator with ", newSeed);
		rng.seed(newSeed);
	}

	void RandomNumberGenerator::seed_randomly() {
		PRINT_DEBUG_CLASS("Generating a random seed");
		size_t randomSeed;
		try {
			std::random_device rd;
			randomSeed = rd();
		} catch (...) {
			RE_ERROR("Failed seeding the random number generator due to the lack of a true random number generator");
		}
		seed(randomSeed);
	}

	[[nodiscard]]
	bool RandomNumberGenerator::random_bool() {
		PRINT_DEBUG_CLASS("Rolling random boolean");
		return random_bool(0.5);
	}

	[[nodiscard]]
	bool RandomNumberGenerator::random_bool(const double dChance) {
		PRINT_DEBUG_CLASS("Rolling random number and checking if it's equal or lower than ", dChance);
		return random<double>(dChance) <= dChance;
	}

	[[nodiscard]]
	double RandomNumberGenerator::random_percentage() {
		PRINT_DEBUG_CLASS("Generating random double");
		return random<double>(1.0);
	}

}
