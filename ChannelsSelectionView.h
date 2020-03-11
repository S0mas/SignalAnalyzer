#pragma once
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include "ChannelsGroupSelectionButton.h"
#include "ChannelStatus.h"

class ChannelsSelectionView : public QGroupBox {
	Q_OBJECT
	int groupsNo_ = 8;
	int channelsPerGroup_;
	QVector<ChannelsGroupSelectionButton*> groupsButtons_;
public:
	ChannelsSelectionView(ChannelStatuses& statuses, const QString& groupDesc, QWidget* parent = nullptr) : QGroupBox(groupDesc, parent) {
		auto layout = new QGridLayout(this);
		auto selectAllButton = new QPushButton("Select All", this);
		auto deselectAllButton = new QPushButton("Deselect All", this);
		channelsPerGroup_ = statuses.size() / groupsNo_;
		for (int groupId = 1; groupId <= groupsNo_; ++groupId) {
			QVector<ChannelSelectionButton*> buttons;
			for (int i = 0; i < channelsPerGroup_; ++i) {
				ChannelId id = (groupId - 1) * channelsPerGroup_ + 1 + i;
				auto button = new ChannelSelectionButton(id, this);
				connect(button, &ChannelSelectionButton::toggled, [&statuses, id](const bool state) { statuses.set(id, state); });
				buttons.push_back(button);
			}
			auto groupConfigButton = new ChannelsGroupSelectionButton(groupId, buttons, this);
			connect(selectAllButton, &QPushButton::clicked, groupConfigButton, &ChannelsGroupSelectionButton::select);
			connect(deselectAllButton, &QPushButton::clicked, groupConfigButton, &ChannelsGroupSelectionButton::deselect);
			layout->addWidget(groupConfigButton, (groupId - 1) / 4, (groupId-1) % 4);
			groupsButtons_.push_back(groupConfigButton);
		}

		layout->addWidget(selectAllButton, 2, 0, 1, 2);
		layout->addWidget(deselectAllButton, 2, 2, 1, 2);
		setLayout(layout);
	}

	QVector<ChannelsGroupSelectionButton*> groupsButtons() {
		return groupsButtons_;
	}


};