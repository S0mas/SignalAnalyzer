#include "ScanRateView.h"
#include "Defines6991.h"
#include <QHBoxLayout>

void ScanRateView::createConnections() noexcept {
	connect(unitComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this]() {
			if (unitComboBox_->currentData().toInt() == static_cast<int>(ScanRateUnitsEnum::HZ)) {
				valueSpinBox_->setSingleStep(HZ_STEP);
				valueSpinBox_->setRange(HZ_STEP, HZ_LIMIT);
				valueSpinBox_->disconnect();
			}
			else if (unitComboBox_->currentData().toInt() == static_cast<int>(ScanRateUnitsEnum::US)) {
				valueSpinBox_->setSingleStep(US_STEP);
				valueSpinBox_->setRange(US_STEP, US_LIMIT);
				connect(valueSpinBox_, &QAbstractSpinBox::editingFinished, [this, step = US_STEP]() {if (valueSpinBox_->value() % step != 0) valueSpinBox_->setValue(valueSpinBox_->value() - valueSpinBox_->value() % step); });
				emit valueSpinBox_->editingFinished();
			}
		}
	);
}

ScanRateView::ScanRateView(QWidget* parent) : QGroupBox("Scan Rate", parent) {
	valueSpinBox_->setMaximumWidth(130);
	valueSpinBox_->setMinimumWidth(130);
	for (auto unit : ScanRateUnitsEnum::TYPES)
		unitComboBox_->addItem(ScanRateUnitsEnum::toString(static_cast<ScanRateUnitsEnum::Type>(unit)), unit);

	auto layout = new QHBoxLayout;
	layout->addWidget(valueSpinBox_);
	layout->addWidget(unitComboBox_);

	setLayout(layout);
	createConnections();
	emit unitComboBox_->currentIndexChanged(0);
}

ScanRateModel ScanRateView::model() const noexcept {
	ScanRateModel value;
	value.units_ = static_cast<ScanRateUnitsEnum::Type>(unitComboBox_->currentData().toInt());
	value.value_ = valueSpinBox_->value();
	return value;
}

void ScanRateView::setModel(ScanRateModel const& model) noexcept {
	unitComboBox_->setCurrentIndex(static_cast<int>(model.units_));
	valueSpinBox_->setValue(model.value_);
}
