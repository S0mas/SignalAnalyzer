#pragma once
#include <QLabel>
#include <QWidget>

class DeviceState;

class StatusView : public QWidget {
	Q_OBJECT
	QLabel* idLabel_ = new QLabel("Id:");
	QLabel* idValue_ = new QLabel;
	QLabel* modeLabel_ = new QLabel("ControlMode:");
	QLabel* modeValue_ = new QLabel;
	QLabel* dataStreamLabel_ = new QLabel("Data Stream:");
	QLabel* dataStreamValue_ = new QLabel;
	QLabel* stateLabel_ = new QLabel("State:");
	QLabel* stateValue_ = new QLabel;
	QLabel* currentTimeLabel_ = new QLabel("Current time:");
	QLabel* currentTimeValue_ = new QLabel;
public:
	StatusView(QWidget* parent = nullptr);
public slots:
	void update(DeviceState const& status) noexcept;
};
