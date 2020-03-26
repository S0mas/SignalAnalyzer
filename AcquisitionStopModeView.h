#pragma once
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QTimeEdit>

class AcquisitionStopModeView : public QGroupBox {
	Q_OBJECT
	QComboBox* modeComboBox_ = new QComboBox;
	QCheckBox* stopOnErrorChckBox_ = new QCheckBox("Stop on Error");
	QLabel* numberOfScansLabel_ = new QLabel("Number of Scans");
	QSpinBox* numberOfScansSpinBox_ = new QSpinBox;
	QLabel* acqusitionTimeLabel_ = new QLabel("Acquisition Time");
	QTimeEdit* acquisitionTimeEdit_ = new QTimeEdit;

	void createConnections() noexcept;
public:
	AcquisitionStopModeView(QWidget* parent = nullptr);
};

