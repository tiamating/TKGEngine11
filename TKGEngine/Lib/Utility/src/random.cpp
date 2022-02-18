
#include "../inc/random.h"

namespace TKGEngine
{
	std::random_device Random::random_device;
	//std::unique_ptr<std::mt19937> Random::random_engine;
	std::mt19937 Random::random_engine;
	std::unique_ptr<std::uniform_real_distribution<double>> Random::dist_real;
	std::unique_ptr<std::uniform_real_distribution<double>> Random::dist_real_11;
	std::unique_ptr<std::uniform_int_distribution<int>> Random::dist_int;
	int Random::seed = 0;

	Random::~Random() = default;

	// ==========================================================
	// ‰Šú‰»ŠÖ”(seed’l‚ğì‚é)
	// ==========================================================
	void Random::Initialize()
	{
		double next_1 = std::nextafter(1.0, (std::numeric_limits<double>::max)());
		seed = static_cast<int>(random_device());
		//random_engine = std::make_unique<std::mt19937>(static_cast<unsigned int>(seed));
		random_engine.seed(static_cast<unsigned int>(seed));
		dist_real = std::make_unique< std::uniform_real_distribution<double>>(0.0, next_1);
		dist_real_11 = std::make_unique< std::uniform_real_distribution<double>>(-1.0, next_1);
		dist_int = std::make_unique<std::uniform_int_distribution<int>>(0, INT_MAX);
	}



}

