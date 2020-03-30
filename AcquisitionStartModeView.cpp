#include "AcquisitionStartModeView.h"
#include "Defines.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

void AcquisitionStartModeView::createConnections() noexcept {
	connect(nowButton_, &QPushButton::clicked, [this]() { startDateTime_->setDateTime(QDateTime::currentDateTime()); });
	connect(modeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this]() {
			if (modeComboBox_->currentData().toInt() == static_cast<int>(AcquisitionStartMode::IMMEDIATE)) {
				startDateTime_->setEnabled(false);
				startDateTime_->setDateTime(startDateTime_->minimumDateTime());
				nowButton_->setEnabled(false);
			}
			else if (modeComboBox_->currentData().toInt() == static_cast<int>(AcquisitionStartMode::PTP_ALARM)) {
				nowButton_->setEnabled(true);
				startDateTime_->setEnabled(true);
			}
		}
	);
}

AcquisitionStartModeView::AcquisitionStartModeView(QWidget * parent) : QGroupBox("Start Mode", parent) {
	modeComboBox_->setMaximumWidth(130);
	for (auto mode : ACQ_START_MODES)
		modeComboBox_->addItem(toString(mode), static_cast<int>(mode));

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
	values.mode_ = static_cast<AcquisitionStartMode>(modeComboBox_->currentData().toInt());
	values.dateTime_ = startDateTime_->dateTime();
	return values;
}

void AcquisitionStartModeView::setModel(AcquisitionStartModeModel const& model) noexcept {
	modeComboBox_->setCurrentIndex(static_cast<int>(model.mode_));
	startDateTime_->setDateTime(model.dateTime_);
}
