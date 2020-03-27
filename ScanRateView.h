#pragma once
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>

struct ScanRateModel;

class ScanRateView : public QGroupBox {
	Q_OBJECT
	int const HZ_LIMIT = 1'000'000;
	int const HZ_STEP = 1;
	int const US_LIMIT = 1'000'000;
	int const US_STEP = 10;
	QSpinBox* valueSpinBox_ = new QSpinBox;
	QComboBox* unitComboBox_ = new QComboBox;
	void createConnections() noexcept;
public:
	ScanRateView(QWidget* parent = nullptr);
	ScanRateModel model() const noexcept;
	void setModel(ScanRateModel const& model) noexcept;
};