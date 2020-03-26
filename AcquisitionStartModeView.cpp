#include "AcquisitionStartModeView.h"
#include "Defines.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

AcquisitionStartModeView::AcquisitionStartModeView(QWidget * parent) : QGroupBox("Start Mode", parent) {
	modeComboBox_->setMaximumWidth(130);
	for (auto mode : ACQ_START_MODES)
		modeComboBox_->insertItem(modeComboBox_->count(), toString(mode), static_cast<int>(mode));

	startModeDate_->setDisplayFormat("dd.MM.yyyy, hh:mm:ss");
	startModeDate_->setMaximumWidth(130);

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(startModeDate_);
	hlayout->addWidget(nowButton_, 1, Qt::AlignRight);
	hlayout->setContentsMargins(0, 0, 0, 0);

	auto layout = new QVBoxLayout;
	layout->addWidget(modeComboBox_);
	layout->addLayout(hlayout);

	setLayout(layout);
	createConnections();
	emit modeComboBox_->currentIndexChanged(0);
}

void AcquisitionStartModeView::createConnections() noexcept {
	connect(nowButton_, &QPushButton::clicked, [this]() { startModeDate_->setDateTime(QDateTime::currentDateTime()); });
	connect(modeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this]() {
			if (modeComboBox_->currentData().toInt() == static_cast<int>(AcquisitionStartMode::IMMEDIATE)) {
				startModeDate_->setEnabled(false);
				startModeDate_->setDateTime(startModeDate_->minimumDateTime());
				nowButton_->setEnabled(false);
			}
			else if(modeComboBox_->currentData().toInt() == static_cast<int>(AcquisitionStartMode::PTP_ALARM)) {
				nowButton_->setEnabled(true);
				startModeDate_->setEnabled(true);
			}
		}
	);
}
