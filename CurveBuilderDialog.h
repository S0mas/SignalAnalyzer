#pragma once
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVector>
#include "ChannelsSelectionView.h"
#include <QDebug>

struct CurveData {
	QString nameId_;
	std::vector<ChannelId> channelsSelected;
	bool single = true;
};

class CurveBuilderDialog : public QDialog {
	Q_OBJECT
	ChannelsSelectionView* selectionView_;
	ChannelStatuses statuses_;
	CurveData curveData_;
	QLineEdit* lineEdit;
	QCheckBox* singleModeCheckBox_;
public:
	CurveBuilderDialog(const ChannelStatuses& statuses, const QString& groupDesc, QWidget* parent = nullptr) : QDialog(parent), statuses_(statuses) {
		selectionView_ = new ChannelsSelectionView(statuses_, groupDesc, parent);
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

		auto layout = new QGridLayout(this);
		auto label = new QLabel("NameId:", this);
		lineEdit = new QLineEdit("curve name", this);
		singleModeCheckBox_ = new QCheckBox("single lines plots", this);
		singleModeCheckBox_->setChecked(true);
		layout->addWidget(label, 0, 0);
		layout->addWidget(lineEdit, 0, 1);
		layout->addWidget(selectionView_, 1, 0, 1, 2);
		layout->addWidget(singleModeCheckBox_, 2, 0, 1, 2);
		layout->addWidget(buttonBox, 3, 0, 1, 2);
		setLayout(layout);
		setAttribute(Qt::WA_DeleteOnClose);

		connect(this, &CurveBuilderDialog::accepted, [this]() {
				curveData_.nameId_ = lineEdit->text();
				curveData_.channelsSelected = statuses_.allEnabled();
				curveData_.single = singleModeCheckBox_->isChecked() || curveData_.channelsSelected.size() == 1;
			}
		);	
	}

	CurveData curveData() const noexcept {
		return curveData_;
	}
};