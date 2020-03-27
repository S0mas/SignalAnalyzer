#include "AcquisitionStopModeView.h"
#include "Defines.h"

#include<QHBoxLayout>
#include<QVBoxLayout>

void AcquisitionStopModeView::createConnections() noexcept {
	connect(modeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this](int const index) {
			if (modeComboBox_->currentData().toInt() == static_cast<int>(AcquisitionStopMode::SCANS_TRESHOLD)) {
				numberOfScansSpinBox_->setEnabled(true);
				acquisitionTimeEdit_->setTime(acquisitionTimeEdit_->minimumTime());
				acquisitionTimeEdit_->setEnabled(false);
			}
			else if (modeComboBox_->currentData().toInt() == static_cast<int>(AcquisitionStopMode::TIME)) {
				numberOfScansSpinBox_->clear();
				numberOfScansSpinBox_->setEnabled(false);
				acquisitionTimeEdit_->setEnabled(true);
			}
			else if (modeComboBox_->currentData().toInt() == static_cast<int>(AcquisitionStopMode::MANUAL)) {
				numberOfScansSpinBox_->clear();
				numberOfScansSpinBox_->setEnabled(false);
				acquisitionTimeEdit_->setTime(acquisitionTimeEdit_->minimumTime());
				acquisitionTimeEdit_->setEnabled(false);
			}
		}
	);
}

AcquisitionStopModeView::AcquisitionStopModeView(QWidget * parent) : QGroupBox("Stop Mode", parent) {
	for (auto mode : ACQ_STOP_MODES)
		modeComboBox_->insertItem(modeComboBox_->count(), toString(mode), static_cast<int>(mode));
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
	values.mode_ = static_cast<AcquisitionStopMode>(modeComboBox_->currentData().toInt());
	values.scansThreshold_ = numberOfScansSpinBox_->value();
	values.time_ = acquisitionTimeEdit_->time();
	values.stopOnError_ = stopOnErrorChckBox_->isChecked();
	return values;
}

void AcquisitionStopModeView::setModel(AcquisitionStopModeModel const& model) noexcept {
	modeComboBox_->setCurrentIndex(static_cast<int>(model.mode_));
	numberOfScansSpinBox_->setValue(model.scansThreshold_);
	acquisitionTimeEdit_->setTime(model.time_);
	stopOnErrorChckBox_->setCheckState(model.stopOnError_ ? Qt::Checked : Qt::Unchecked);
}
