#pragma once
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

class IdentificationGroupView : public QGroupBox {
	Q_OBJECT
	QLabel* serialNumber;
	QLabel* firmwareRevision;
	QLabel* driverRevision;
public:
	IdentificationGroupView(QWidget* parent = nullptr) : QGroupBox("Identification Data", parent) {
		serialNumber = new QLabel("12345678", this);
		firmwareRevision = new QLabel("3.148", this);
		driverRevision = new QLabel("2.176", this);

		auto layout = new QGridLayout(this);	
		layout->addWidget(new QLabel("Serial number:", this), 0, 0);
		layout->addWidget(serialNumber, 0, 1);
		layout->addWidget(new QLabel("Firmware revision:", this), 1, 0);
		layout->addWidget(firmwareRevision, 1, 1);
		layout->addWidget(new QLabel("Driver revision:", this), 2, 0);
		layout->addWidget(driverRevision, 2, 1);	
		setLayout(layout);
	}
};