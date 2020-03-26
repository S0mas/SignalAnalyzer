#pragma once
#include <QGroupBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QPushButton>

class AcquisitionStartModeView : public QGroupBox {
	Q_OBJECT
	QComboBox* modeComboBox_ = new QComboBox;
	QDateTimeEdit* startModeDate_ = new QDateTimeEdit;
	QPushButton* nowButton_ = new QPushButton("Now");

	void createConnections() noexcept;
public:
	AcquisitionStartModeView(QWidget* parent = nullptr);
};

