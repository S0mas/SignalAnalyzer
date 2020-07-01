#pragma once
#include <QtGlobal>
#include <QDebug>

class DataGenerator {
public:
	DataGenerator() {}

	std::vector<double> generateRandomData(const int dataSize, const double maxValue = 1, const int minSameStateLength = 1, int maxSameStateLength = -1) const noexcept {
		maxSameStateLength = maxSameStateLength < 1 ? dataSize : maxSameStateLength;
		if (minSameStateLength > maxSameStateLength)
			qDebug() << "Error! Check generation function call";
		std::vector<double> data;
		data.reserve(dataSize);
		while (data.size() != dataSize) {
			long long int randomValue = rand() % (static_cast<int>(maxValue) + 1);
			int randomLenght = rand() % (maxSameStateLength - minSameStateLength + 1) + minSameStateLength;
			int spaceLeft = dataSize - data.size();
			data.insert(data.end(), randomLenght > spaceLeft ? spaceLeft : randomLenght, randomValue);
		}
		return data;
	}
};
