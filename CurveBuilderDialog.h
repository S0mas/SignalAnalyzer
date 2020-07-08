#pragma once
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QVector>
#include "../WizardFramework/Common/include/gui/ChannelsSelectionView.h"
#include "../WizardFramework/Common/include/Device6991/Defines6991.h"
#include <QDebug>

struct CurveData {
	QString nameId_;
	std::vector<uint32_t> channelsSelected;
	bool vectorize;
	uint32_t samplesNo_;
	uint32_t firstSampleId_;
	bool isRealTimeDataSource_;
	DeviceType deviceType_;
};

class CurveBuilderDialog : public QDialog {
	Q_OBJECT
	ChannelsSelectionView* selectionView_;
	ChannelStatuses statuses_;
	CurveData curveData_;
	QLineEdit* lineEdit;
	QCheckBox* vectorizeCheckBox_;
	QSpinBox* spinBoxSamplesNo_ = nullptr;
	QSpinBox* spinBoxFirstSampleId_ = nullptr;
public:
	CurveBuilderDialog(DeviceType const deviceType, const ChannelStatuses& statuses, const QString& groupDesc, bool const isRealTimeDataSource, QWidget* parent = nullptr) : QDialog(parent), statuses_(statuses) {
		selectionView_ = new ChannelsSelectionView(statuses_, 8, groupDesc, 4);
		for(auto groupButton : selectionView_->groupsButtons())
			for (auto channelbutton : groupButton->channelButtons())
				channelbutton->setEnabled(statuses_.status(channelbutton->id()));

		statuses_.disableAll();
		for (auto groupButton : selectionView_->groupsButtons())
			for (auto channelbutton : groupButton->channelButtons())
				channelbutton->setChecked(statuses_.status(channelbutton->id()));

		auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);


		auto label = new QLabel("NameId:");
		lineEdit = new QLineEdit("Channel");

		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(label);
		hlayout->addWidget(lineEdit);

		auto layout = new QVBoxLayout;
		layout->addLayout(hlayout);
		if (!isRealTimeDataSource) {
			label = new QLabel("Samples Number:");
			spinBoxSamplesNo_ = new QSpinBox;
			spinBoxSamplesNo_->setMaximum(50000);
			spinBoxSamplesNo_->setMinimum(1);
			hlayout = new QHBoxLayout;
			hlayout->addWidget(label);
			hlayout->addWidget(spinBoxSamplesNo_);
			layout->addLayout(hlayout);
			

			label = new QLabel("Start with sample id:");
			spinBoxFirstSampleId_ = new QSpinBox;
			spinBoxFirstSampleId_->setMaximum(50000);
			spinBoxFirstSampleId_->setMinimum(0);
			hlayout = new QHBoxLayout;
			hlayout->addWidget(label);
			hlayout->addWidget(spinBoxFirstSampleId_);
			layout->addLayout(hlayout);
		}

		vectorizeCheckBox_ = new QCheckBox("Vectorize");
		vectorizeCheckBox_->setChecked(false);

		layout->addWidget(selectionView_);
		if (deviceType == DeviceType::_6111)
			layout->addWidget(vectorizeCheckBox_);
		layout->addWidget(buttonBox);
		setLayout(layout);
		setAttribute(Qt::WA_DeleteOnClose);

		curveData_.deviceType_ = deviceType;
		curveData_.isRealTimeDataSource_ = isRealTimeDataSource;

		connect(this, &CurveBuilderDialog::accepted, [this]() {
				curveData_.nameId_ = lineEdit->text();
				curveData_.channelsSelected = statuses_.allEnabled();
				curveData_.vectorize = vectorizeCheckBox_->isChecked();
				if (spinBoxSamplesNo_)
					curveData_.samplesNo_ = spinBoxSamplesNo_->value();
				if(spinBoxFirstSampleId_)
					curveData_.firstSampleId_ = spinBoxFirstSampleId_->value();
			}
		);	
	}

	CurveData curveData() const noexcept {
		return curveData_;
	}
signals:
	void curveToAdd(QString const& source, CurveData const& curveData) const;
};