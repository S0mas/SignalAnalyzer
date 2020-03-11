#pragma once
#include <QRandomGenerator>
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
			long long int randomValue = QRandomGenerator::global()->bounded(maxValue + 1);
			int randomLenght = QRandomGenerator::global()->bounded(minSameStateLength, maxSameStateLength + 1);
			int spaceLeft = dataSize - data.size();
			data.insert(data.end(), randomLenght > spaceLeft ? spaceLeft : randomLenght, randomValue);
		}
		return data;
	}
};
