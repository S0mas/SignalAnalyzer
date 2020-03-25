#pragma once
#include <QCheckBox>
#include <QRadioButton>
#include <QVBoxLayout>

#include <vector>
#include <iostream>

#include "AcquisitionGroupView.h"
#include "ChannelsSelectionView.h"
#include "Controller.h"
#include "IdentificationGroupView.h"
#include "Plot.h"
#include "CurveBuilderDialog.h"
#include "DataController.h"

class View : public QWidget {
	Q_OBJECT
	AcquisitionGroupView* acquisitionGroupView_;
	ChannelsSelectionView* channelsSelectionView_;
	IdentificationGroupView* identificationGroupView_;
	QPushButton* latchButton_;
	PlotView* plot_;
	CurveBuilderDialog* addCurveDialog_;
	ChannelStatuses statuses_{ 256 };
	DataController* dataController_;
	DataAcquisitor dataAcq;

	QWidget* createDlLinClockkController() {
		auto layout = new QVBoxLayout(this);
		for (const auto frequency : FREQUENCIES) {
			auto button = new QRadioButton(toString(frequency), this);
			button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
			if(button->text() == toString(Frequency::_2MHz))
				button->setChecked(true);
			connect(button, &QRadioButton::clicked, [this, frequency]() { emit setDlLinkClockFrequency(frequency); });
			layout->addWidget(button);
		}

		auto group = new QGroupBox("DL Link Clock Frequency");
		group->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		group->setLayout(layout);
		return group;
	}
private slots:
	void openCurveBuilderDialog() {
		auto curveBuilderDialog = new CurveBuilderDialog(statuses_, "Channels Selection", this);
		connect(curveBuilderDialog, &CurveBuilderDialog::accepted, [this, curveBuilderDialog]() { addCurve(curveBuilderDialog->curveData()); });
		curveBuilderDialog->open();
	}

	void addCurve(const CurveData& data) const noexcept {
		if (data.single)
			for (auto const index : data.channelsSelected)
				plot_->addCurveSingle(QString("%1 %2").arg(data.nameId_).arg(index), dataController_->dataGetter(index));
		else
			plot_->addCurveVec(data.nameId_, dataController_->dataGetter(data.channelsSelected));
	}

	void addCurveDialogUpdate() const {}
public:
	View(QWidget* parent = nullptr) : QWidget(parent) {
		identificationGroupView_ = new IdentificationGroupView(this);
		identificationGroupView_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		acquisitionGroupView_ = new AcquisitionGroupView(this);
		acquisitionGroupView_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		channelsSelectionView_ = new ChannelsSelectionView(statuses_, "Channels Controller", this);
		channelsSelectionView_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		latchButton_ = new QPushButton("Latch", this);
		latchButton_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		dataController_ = new DataController(this);
		dataController_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		plot_ = new PlotView(this);
		plot_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

		connect(acquisitionGroupView_, &AcquisitionGroupView::startAcquisition, this, &View::startAcquisition);
		connect(acquisitionGroupView_, &AcquisitionGroupView::startAcquisition, [this]() {channelsSelectionView_->setDisabled(true); });
		connect(acquisitionGroupView_, &AcquisitionGroupView::stopAcquisition, this, &View::stopAcquisition);
		connect(acquisitionGroupView_, &AcquisitionGroupView::stopAcquisition, [this]() {channelsSelectionView_->setEnabled(true); });

		connect(this, &View::acquisitionStarted, acquisitionGroupView_, &AcquisitionGroupView::acquisitionStarted);
		connect(this, &View::acquisitionStarted, [this]() {channelsSelectionView_->setDisabled(true); });
		connect(this, &View::acquisitionStopped, acquisitionGroupView_, &AcquisitionGroupView::acquisitionStopped);
		connect(this, &View::acquisitionStopped, [this]() {channelsSelectionView_->setEnabled(true); });

		connect(plot_, &PlotView::addCurveActionStarted, this, &View::openCurveBuilderDialog);

		auto vlayout = new QVBoxLayout(this);
		vlayout->addWidget(identificationGroupView_);
		vlayout->addWidget(acquisitionGroupView_);
		vlayout->addWidget(channelsSelectionView_);
		vlayout->addWidget(createDlLinClockkController());
		vlayout->addWidget(latchButton_);
		vlayout->addWidget(dataController_);

		auto controllersWidget = new QWidget;
		controllersWidget->setLayout(vlayout);

		auto splitter = new QSplitter(Qt::Orientation::Horizontal);

		controllersWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		splitter->addWidget(controllersWidget);
		splitter->addWidget(plot_);
		splitter->setContentsMargins(0, 0, 0, 0);

		auto layout = new QVBoxLayout;
		layout->addWidget(splitter);
		setLayout(layout);
	}
signals:
	//output
	void toogleChannel(const unsigned int channelId, const bool state) const;
	void toogleChannelsGroup(const unsigned int groupId, const bool state) const;
	void toogleAllChannels(const bool state) const;
	void startAcquisition() const;
	void stopAcquisition() const;
	void setDlLinkClockFrequency(const Frequency frequency) const;

	//input
	void acquisitionStarted() const;
	void acquisitionStopped() const;
};