#include "AcquisitionStartModeView.h"
#include "Defines6991.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

void AcquisitionStartModeView::createConnections() noexcept {
	connect(nowButton_, &QPushButton::clicked, [this]() { startDateTime_->setDateTime(QDateTime::currentDateTime()); });
	connect(modeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this]() {
			if (modeComboBox_->currentData().toInt() == AcquisitionStartModeEnum::IMMEDIATE) {
				startDateTime_->setEnabled(false);
				startDateTime_->setDateTime(startDateTime_->minimumDateTime());
				nowButton_->setEnabled(false);
			}
			else if (modeComboBox_->currentData().toInt() == AcquisitionStartModeEnum::PTP_ALARM) {
				nowButton_->setEnabled(true);
				startDateTime_->setEnabled(true);
			}
		}
	);
}

AcquisitionStartModeView::AcquisitionStartModeView(QWidget * parent) : QGroupBox("Start Mode", parent) {
	modeComboBox_->setMaximumWidth(130);
	for (auto mode : AcquisitionStartModeEnum::TYPES)
		modeComboBox_->addItem(AcquisitionStartModeEnum::toString(mode), mode);

	startDateTime_->setDisplayFormat("dd.MM.yyyy, hh:mm:ss");
	startDateTime_->setMaximumWidth(130);

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(startDateTime_);
	hlayout->addWidget(nowButton_, 1, Qt::AlignRight);
	hlayout->setContentsMargins(0, 0, 0, 0);

	auto layout = new QVBoxLayout;
	layout->addWidget(modeComboBox_);
	layout->addLayout(hlayout);

	setLayout(layout);
	createConnections();
	emit modeComboBox_->currentIndexChanged(0);
}

AcquisitionStartModeModel AcquisitionStartModeView::model() const noexcept {
	AcquisitionStartModeModel values;
	values.mode_ = static_cast<AcquisitionStartModeEnum::Type>(modeComboBox_->currentData().toInt());
	if (values.mode_ == AcquisitionStartModeEnum::PTP_ALARM)
		values.ptpAlarm_ = { 0, 0 }; // TODO calculate ptp alarm time from startDateTime_->dateTime();
	return values;
}

void AcquisitionStartModeView::setModel(AcquisitionStartModeModel const& model) noexcept {
	if(model.mode_)
		modeComboBox_->setCurrentIndex(*model.mode_);
	if(model.ptpAlarm_)
		startDateTime_->setDateTime(QDateTime()); // TODO calculate QDateTime from ptp alarm
}
