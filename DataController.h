#pragma once
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include "ChannelsSelectionView.h"
#include "DataGenerator.h"
#include "DataAcquisitor.h"
#include "Defines.h"
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QRadioButton>
#include <QDebug>
#include <vector>

class DataController : public QGroupBox {
	Q_OBJECT
	QRadioButton* isGeneratorSelected;
	QRadioButton* isAcquisitorSelected;
	QThread generatorThread;
	ChannelsSelectionView* view_ = nullptr;
	ChannelStatuses statuses_ = {256};
	DataGenerator dataGen_;
	DataAcquisitor dataAcq_;
	bool guiSignalsControlEnabled = true;
public:
	DataController(QWidget* parent = nullptr) : QGroupBox("Data Controller", parent) {
		auto layout = new QHBoxLayout(this);
		isAcquisitorSelected = new QRadioButton("use acquisiton", this);
		isGeneratorSelected = new QRadioButton("use software generator", this);
		isGeneratorSelected->setChecked(true);
		layout->addWidget(isAcquisitorSelected);
		layout->addWidget(isGeneratorSelected);
		setLayout(layout);
	}

	std::vector<double> generateDataSingleLine() const noexcept {
		return dataGen_.generateRandomData(1000, 1, 5, 15);
	}

	std::vector<double> generateDataVec(const long long int max) const noexcept {
		return dataGen_.generateRandomData(1000, max, 30, 90);
	}

	std::vector<double> getData(const ChannelId channelId) const noexcept {
		return isGeneratorSelected->isChecked() ? generateDataSingleLine() : dataAcq_.data(channelId);
	}

	std::vector<double> getData(const std::vector<ChannelId>& channelId) const noexcept {
		return isGeneratorSelected->isChecked() ? generateDataVec(0xFFFF) : dataAcq_.data(channelId);
	}

	auto dataGetter(const ChannelId channelId) const noexcept {
		return [this, channelId]() { return getData(channelId); };
	}

	auto dataGetter(const std::vector<ChannelId>& channelsIds) const noexcept {
		return [this, channelsIds]() { return getData(channelsIds); };
	}
};