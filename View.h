#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>
#include <QVBoxLayout>

#include <memory>
#include <vector>
#include "Plot.h"
#include "CurveBuilderDialog.h"
#include "MyPlotCurve.h"
#include "MyPlotIntervalCurve.h"
#include "DataController.h"
#include "SettingsToolbar.h"

class View : public QWidget {
	Q_OBJECT
	PlotView* plot_;
	CurveBuilderDialog* addCurveDialog_;
	DataController* dataController_ = new DataController(this);

	std::map<QString, ChannelStatuses> devs_;
private slots:
	void openCurveBuilderDialog() {
		auto sourceList = dataController_->connectedDevices();
		if (sourceList.isEmpty()) {
			qDebug() << "No devices connected or loaded.";
			return;
		}
		bool pressedOk = false;
		auto source = QInputDialog::getItem(this, "Select the source of the signal data", "Source", sourceList, 0, false, &pressedOk);
		if (pressedOk) {
			bool isRealTimeDataSource = dataController_->isRealTimeSource(source);
			DeviceType deviceType = dataController_->deviceType(source);
			auto curveBuilderDialog = new CurveBuilderDialog(deviceType, dataController_->statuses(source), "Channels Selection", isRealTimeDataSource, this);
			connect(curveBuilderDialog, &CurveBuilderDialog::accepted, [this, source, curveBuilderDialog]() { addCurve(source, curveBuilderDialog->curveData()); });
			curveBuilderDialog->exec();
		}
	}

	void addCurve(QString const& deviceId, const CurveData& data) noexcept {
		DeviceType deviceType = dataController_->deviceType(deviceId);
		bool isRealTimeDataSource = dataController_->isRealTimeSource(deviceId);
		if (isRealTimeDataSource) {
			if (!data.vectorize) {
				for (auto const index : data.channelsSelected)
					if (deviceType == DeviceType::_6111)
						plot_->addSingleBitSignal(QString("%1 %2").arg(data.nameId_).arg(index), dataController_->createDataEmitter(deviceId, index));
					else if (deviceType == DeviceType::_6132)
						plot_->addComplexSignalWave(QString("%1 %2").arg(data.nameId_).arg(index), dataController_->createDataEmitter(deviceId, index));
			}
			else
				plot_->addComplexSignalBlock(data.nameId_, dataController_->createDataEmitter(deviceId, data.channelsSelected));
		}
		else {
			if (!data.vectorize) {
				for (auto const index : data.channelsSelected)
					if (deviceType == DeviceType::_6111)
						plot_->addStaticSingleBitSignal(QString("%1 %2").arg(data.nameId_).arg(index), dataController_->staticData(deviceId, index, data.samplesNo_, data.firstSampleId_));
					else if (deviceType == DeviceType::_6132)
						plot_->addStaticComplexSignalWave(QString("%1 %2").arg(data.nameId_).arg(index), dataController_->staticData(deviceId, index, data.samplesNo_, data.firstSampleId_));
			}
			else
				plot_->addStaticComplexSignalBlock(data.nameId_, dataController_->staticData(deviceId, data.channelsSelected,  data.samplesNo_, data.firstSampleId_));
		}		
	} 

	void addCurveDialogUpdate() const {}
public:
	View(QWidget* parent = nullptr) : QWidget(parent) {
		plot_ = new PlotView(this);
		connect(plot_, &PlotView::addCurveActionStarted, this, &View::openCurveBuilderDialog);
		connect(dataController_, &DataController::logMsg, [](auto const& msg) { qDebug() << "LOG: " << msg; });

		auto splitter = new QSplitter(Qt::Orientation::Horizontal);
		splitter->addWidget(plot_);
		splitter->setContentsMargins(0, 0, 0, 0);


		
		auto layout = new QVBoxLayout;
		layout->setMenuBar(new SettingsToolbar(plot_->myPlot(), dataController_));
		layout->addWidget(splitter);
		setLayout(layout);
	}
signals:
};