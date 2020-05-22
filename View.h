#pragma once
#include <QCheckBox>
#include <QRadioButton>
#include <QVBoxLayout>

#include <vector>
#include "Plot.h"
#include "CurveBuilderDialog.h"
#include "DataController.h"
#include "DataCollectorServer.h"
#include "PacketReading.h"

class View : public QWidget {
	Q_OBJECT
	PlotView* plot_;
	CurveBuilderDialog* addCurveDialog_;
	DataController* dataController_;
	DataAcquisitor dataAcq;
	DataCollectorServer* dataCollector_ = new DataCollectorServer(1, new PacketReading<SignalDataPacket::Header, float>, this);
	std::vector<DataCollectorClient*> clients_;
private slots:
	void openCurveBuilderDialog() {
		//auto curveBuilderDialog = new CurveBuilderDialog(statuses_, "Channels Selection", this);
		//connect(curveBuilderDialog, &CurveBuilderDialog::accepted, [this, curveBuilderDialog]() { addCurve(curveBuilderDialog->curveData()); });
		//curveBuilderDialog->open();
	}

	void addCurve(const CurveData& data) const noexcept {
		if (data.single)
			for (auto const index : data.channelsSelected)
				plot_->addCurveSingle(QString("%1 %2").arg(data.nameId_).arg(index), dataController_->dataGetter(index));
		else
			plot_->addCurveVec(data.nameId_, dataController_->dataGetter(data.channelsSelected));
	}

	void addCurveDialogUpdate() const {}
	void addNewClient(DataCollectorClient* client) {
		clients_.push_back(client);
	}
public:
	View(QWidget* parent = nullptr) : QWidget(parent) {
		plot_ = new PlotView(this);
		connect(plot_, &PlotView::addCurveActionStarted, this, &View::openCurveBuilderDialog);
		connect(dataCollector_, &DataCollectorServer::newClientConnected, this, &View::addNewClient);
		connect(dataCollector_, &DataCollectorServer::logMsg, [](auto const& msg) { qDebug() << msg; });

		auto splitter = new QSplitter(Qt::Orientation::Horizontal);
		splitter->addWidget(plot_);
		splitter->setContentsMargins(0, 0, 0, 0);

		auto layout = new QVBoxLayout;
		layout->addWidget(splitter);
		setLayout(layout);
	}
signals:
};