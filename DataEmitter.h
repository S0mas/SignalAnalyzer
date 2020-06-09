#pragma once
#include <functional>
#include <vector>
#include <QObject>

class DataEmitter : public QObject {
	Q_OBJECT
	std::function<std::vector<double>()> dataGetter_;
public:
	DataEmitter(std::function<std::vector<double>()> const& dataGetter, QObject* parent = nullptr) : QObject(parent), dataGetter_(dataGetter) {}
public slots:
	void handleDataRequest() {
		emit data(dataGetter_());
	}
signals:
	void data(std::vector<double> const& data) const;
};
