#pragma once

#include <QDialog>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

class SetScaleDialog : public QDialog {
public:
	struct Result {
		bool autoScale_ = true;
		double min_ = 0;
		double max_ = 0;
	};
private:
	Result result_;
public:
	SetScaleDialog(QWidget* parent = nullptr) : QDialog(parent) {
		auto autoScaleRadioButton = new QRadioButton("Auto Scale");
		auto manualScaleRadioButton = new QRadioButton("Manual Scale");

		auto minSpinBox = new QDoubleSpinBox;
		auto maxSpinBox = new QDoubleSpinBox;
		minSpinBox->setMinimum(-1001);
		minSpinBox->setMaximum(1000);
		maxSpinBox->setMinimum(-1000);
		maxSpinBox->setMaximum(1001);
		auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		minSpinBox->setEnabled(false);
		maxSpinBox->setEnabled(false);

		connect(buttonBox, &QDialogButtonBox::accepted, 
			[this, autoScaleRadioButton, minSpinBox, maxSpinBox]() {
				result_.autoScale_ = autoScaleRadioButton->isChecked();
				if (!result_.autoScale_) {
					result_.min_ = minSpinBox->value();
					result_.max_ = maxSpinBox->value();
				}
				accept();
			}
		);
		connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
		connect(manualScaleRadioButton, &QRadioButton::toggled, 
			[this, minSpinBox, maxSpinBox](bool const checked) {
				minSpinBox->setEnabled(checked);
				maxSpinBox->setEnabled(checked);
			}
		);
		connect(minSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [maxSpinBox](double const value) {if (value >= maxSpinBox->value()) maxSpinBox->setValue(value + 1); });
		connect(maxSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [minSpinBox](double const value) {if (value <= minSpinBox->value()) minSpinBox->setValue(value - 1); });

		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(autoScaleRadioButton);
		hlayout->addWidget(manualScaleRadioButton);
		auto vlayout = new QVBoxLayout;
		vlayout->addLayout(hlayout);
		hlayout = new QHBoxLayout;
		hlayout->addWidget(minSpinBox);
		hlayout->addWidget(maxSpinBox);
		vlayout->addLayout(hlayout);
		vlayout->addWidget(buttonBox);
		setLayout(vlayout);
		autoScaleRadioButton->setChecked(true);
	}
public:
	Result result() const noexcept {
		return result_;
	}
};