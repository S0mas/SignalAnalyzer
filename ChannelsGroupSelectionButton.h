#pragma once
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include "ChannelStatus.h"

class ChannelsGroupSelectionButton : public QPushButton {
	Q_OBJECT
	unsigned int groupId_;
	QPushButton* selectGroupButton_;
	QPushButton* deselectGroupButton_;
	QVector<ChannelSelectionButton*> channelsButtons_;

	QDialog* selectionDialog_;
	void createSelectionDialog() {
		selectionDialog_ = new QDialog(this);
		selectionDialog_->setModal(true);
		auto layout = new QGridLayout(this);
		for (int i = 0; i < channelsButtons_.size(); ++i)
			layout->addWidget(channelsButtons_[i], i / 8, i % 8);
		layout->addWidget(selectGroupButton_, 4, 0, 1, 4);
		layout->addWidget(deselectGroupButton_, 4, 4, 1, 4);
		selectionDialog_->setLayout(layout);
		connect(this, &QPushButton::clicked, selectionDialog_, &QDialog::open);
	}
public:
	ChannelsGroupSelectionButton(const unsigned int groupId, QVector<ChannelSelectionButton*> channelButtons, QWidget* parent = nullptr)
		: QPushButton(QString("Group %1").arg(groupId), parent),
		groupId_(groupId),
		channelsButtons_(channelButtons){
		
		selectGroupButton_ = new QPushButton("Select Group", this);
		deselectGroupButton_ = new QPushButton("Deselect Group", this);

		connect(deselectGroupButton_, &QPushButton::clicked, this, &ChannelsGroupSelectionButton::deselect);
		connect(selectGroupButton_, &QPushButton::clicked, this, &ChannelsGroupSelectionButton::select);
		createSelectionDialog();
	}
public:
	QVector<ChannelSelectionButton*> channelButtons() {
		return channelsButtons_;
	}
public slots:
	void deselect() const {
		for (auto channelButton : channelsButtons_)
			if (channelButton->isEnabled()) 
				channelButton->setChecked(false);
	}

	void select() const {
		for (auto channelButton : channelsButtons_)
			if (channelButton->isEnabled())
				channelButton->setChecked(true);
	}
};