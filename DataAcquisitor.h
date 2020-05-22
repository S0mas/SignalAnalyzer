#pragma once
#include "DataGenerator.h"
#include "Defines.h"
#include <vector>

class DataAcquisitor {
	DataGenerator generator;//to be removed
public:
	DataAcquisitor() {}

	std::vector<double> data(const uint32_t id) const noexcept {
		return generator.generateRandomData(1000, 1, 5, 15);
	}

	std::vector<double> data(const std::vector<uint32_t> id) const noexcept {
		return generator.generateRandomData(1000, 0xFFFF, 30, 90);
	}
};