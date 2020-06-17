#pragma once
#include <functional>
#include <vector>
#include <QObject>
#include "SignalDataPacket.h"

class DataEmitter : public QObject {
	Q_OBJECT
	std::function<std::pair<std::vector<double>, std::vector<Timestamp6991>>()> dataGetter_;
public:
	DataEmitter(std::function<std::pair<std::vector<double>, std::vector<Timestamp6991>>()> const& dataGetter, QObject* parent = nullptr) : QObject(parent), dataGetter_(dataGetter) {}
public slots:
	void handleDataRequest() {
		emit data(dataGetter_());
	}
signals:
	void data(std::pair<std::vector<double>, std::vector<Timestamp6991>> const& data) const;
};
