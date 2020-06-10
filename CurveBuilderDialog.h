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
#include <QDebug>

struct CurveData {
	QString nameId_;
	std::vector<uint32_t> channelsSelected;
	bool single = true;
	uint32_t samplesNo_;
	uint32_t firstSampleId_;
};

class CurveBuilderDialog : public QDialog {
	Q_OBJECT
	ChannelsSelectionView* selectionView_;
	ChannelStatuses statuses_;
	CurveData curveData_;
	QLineEdit* lineEdit;
	QCheckBox* singleModeCheckBox_;
	QSpinBox* spinBoxSamplesNo_ = nullptr;
	QSpinBox* spinBoxFirstSampleId_ = nullptr;
public:
	CurveBuilderDialog(const ChannelStatuses& statuses, const QString& groupDesc, bool const isRealTimeDataSource, QWidget* parent = nullptr) : QDialog(parent), statuses_(statuses) {
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
		lineEdit = new QLineEdit("curve name");

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

		singleModeCheckBox_ = new QCheckBox("single lines plots");
		singleModeCheckBox_->setChecked(true);
	
		layout->addWidget(selectionView_);
		layout->addWidget(singleModeCheckBox_);
		layout->addWidget(buttonBox);
		setLayout(layout);
		setAttribute(Qt::WA_DeleteOnClose);

		connect(this, &CurveBuilderDialog::accepted, [this]() {
				curveData_.nameId_ = lineEdit->text();
				curveData_.channelsSelected = statuses_.allEnabled();
				curveData_.single = singleModeCheckBox_->isChecked() || curveData_.channelsSelected.size() == 1;
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
};