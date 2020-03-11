#pragma once
#include "DataGenerator.h"
#include "Defines.h"
#include <vector>

class DataAcquisitor {
	DataGenerator generator;//to be removed
public:
	DataAcquisitor() {}

	std::vector<double> data(const ChannelId id) const noexcept {
		return generator.generateRandomData(1000, 1, 5, 15);
	}

	std::vector<double> data(const std::vector<ChannelId> id) const noexcept {
		return generator.generateRandomData(1000, 0xFFFF, 30, 90);
	}
};