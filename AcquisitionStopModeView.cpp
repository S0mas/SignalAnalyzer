#include "AcquisitionStopModeView.h"
#include "Defines6991.h"

#include<QHBoxLayout>
#include<QVBoxLayout>

void AcquisitionStopModeView::createConnections() noexcept {
	connect(modeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this](int const index) {
			if (modeComboBox_->currentData().toInt() == AcquisitionStopModeEnum::SCANS_TRESHOLD) {
				numberOfScansSpinBox_->setEnabled(true);
				acquisitionTimeEdit_->setTime(acquisitionTimeEdit_->minimumTime());
				acquisitionTimeEdit_->setEnabled(false);
			}
			else if (modeComboBox_->currentData().toInt() == AcquisitionStopModeEnum::TIME) {
				numberOfScansSpinBox_->clear();
				numberOfScansSpinBox_->setEnabled(false);
				acquisitionTimeEdit_->setEnabled(true);
			}
			else if (modeComboBox_->currentData().toInt() == AcquisitionStopModeEnum::MANUAL) {
				numberOfScansSpinBox_->clear();
				numberOfScansSpinBox_->setEnabled(false);
				acquisitionTimeEdit_->setTime(acquisitionTimeEdit_->minimumTime());
				acquisitionTimeEdit_->setEnabled(false);
			}
		}
	);
}

AcquisitionStopModeView::AcquisitionStopModeView(QWidget * parent) : QGroupBox("Stop Mode", parent) {
	for (auto mode : AcquisitionStopModeEnum::TYPES)
		modeComboBox_->addItem(AcquisitionStopModeEnum::toString(mode), mode);
	modeComboBox_->setMaximumWidth(130);

	numberOfScansSpinBox_->setRange(1, 1000000);
	numberOfScansSpinBox_->setSingleStep(1);
	numberOfScansSpinBox_->setMaximumWidth(130);

	acquisitionTimeEdit_->setDisplayFormat("hh:mm:ss:zzz");
	acquisitionTimeEdit_->setMaximumWidth(130);

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(modeComboBox_);
	hlayout->addWidget(stopOnErrorChckBox_);
	hlayout->setContentsMargins(0, 0, 0, 0);
	auto layout = new QVBoxLayout;
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(numberOfScansLabel_);
	hlayout->addWidget(numberOfScansSpinBox_);
	hlayout->setContentsMargins(0, 0, 0, 0);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(acqusitionTimeLabel_);
	hlayout->addWidget(acquisitionTimeEdit_);
	hlayout->setContentsMargins(0, 0, 0, 0);
	layout->addLayout(hlayout);

	setLayout(layout);
	createConnections();
	emit modeComboBox_->currentIndexChanged(0);
}

AcquisitionStopModeModel AcquisitionStopModeView::model() const noexcept {
	AcquisitionStopModeModel values;
	if (modeComboBox_->currentData().toInt() == AcquisitionStopModeEnum::TIME)
		values.scansThreshold_ = 0; // TODO calculate treshold from time acquisitionTimeEdit_->time();
	else if (modeComboBox_->currentData().toInt() == AcquisitionStopModeEnum::SCANS_TRESHOLD)
		values.scansThreshold_ = numberOfScansSpinBox_->value();
	else
		values.scansThreshold_ = 0;
	values.stopOnError_ = stopOnErrorChckBox_->isChecked();
	return values;
}

void AcquisitionStopModeView::setModel(AcquisitionStopModeModel const& model) noexcept {
	if (model.scansThreshold_) {
		if(*model.scansThreshold_ == 0)
			modeComboBox_->setCurrentIndex(AcquisitionStopModeEnum::MANUAL);
		else
			modeComboBox_->setCurrentIndex(AcquisitionStopModeEnum::SCANS_TRESHOLD);
		numberOfScansSpinBox_->setValue(*model.scansThreshold_);
		acquisitionTimeEdit_->setTime(acquisitionTimeEdit_->minimumTime());
	}
	if(model.stopOnError_)
		stopOnErrorChckBox_->setCheckState(*model.stopOnError_ ? Qt::Checked : Qt::Unchecked);
}
