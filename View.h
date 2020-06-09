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

class View : public QWidget {
	Q_OBJECT
	PlotView* plot_;
	CurveBuilderDialog* addCurveDialog_;
	DataController* dataController_ = new DataController(this);

	std::map<QString, ChannelStatuses> devs_;
private slots:
	void openCurveBuilderDialog() {
		auto sourceList = dataController_->connectedDevices();
		sourceList.push_back("NoDevice");
		auto source = QInputDialog::getItem(this, "Select the source of the signal data", "Source", sourceList, 0, false);

		

		auto curveBuilderDialog = new CurveBuilderDialog(dataController_->statuses(source), "Channels Selection", this);
		connect(curveBuilderDialog, &CurveBuilderDialog::accepted, [this, source, curveBuilderDialog]() { addCurve(source, curveBuilderDialog->curveData()); });
		curveBuilderDialog->open();
	}

	void addCurve(QString const& deviceId, const CurveData& data) noexcept {
		DeviceType deviceType = dataController_->deviceType(deviceId);
		if (data.single) {
			for (auto const index : data.channelsSelected)
				if (deviceType == DeviceType::_6111)
					plot_->addSingleBitSignal(QString("%1 %2").arg(data.nameId_).arg(index), dataController_->createDataEmitter(deviceId, index));
				else if (deviceType == DeviceType::_6132)
					plot_->addComplexSignalWave(QString("%1 %2").arg(data.nameId_).arg(index), dataController_->createDataEmitter(deviceId, index));
		}
		else
			//plot_->addComplexSignalBlock(data.nameId_, dataController_->createDataEmitter(deviceId, data.channelsSelected));
		{
			for (auto const index : data.channelsSelected) {
				std::unique_ptr<MyPlotAbstractCurve> curve = std::make_unique<MyPlotIntervalCurve>(data.nameId_, new MyIntervalSymbol2, plot_->myPlot(), false);
				curve->handleData(dataController_->data(deviceId, index));
				plot_->addStaticCurve(curve);
			}

		}
			
	} 

	QToolBar* createToolBar() {
		auto menu = new QMenu("Settings", this);
		menu->addAction("Set Refresh Time Interval",
			[this]() {
				auto dialog = new QInputDialog(this);
				plot_->setRefreshTimeInterval(dialog->getInt(this, "Setting Refresh Time Interval", "Refresh Time Interval[ms, <10, 2000>]", plot_->refreshTimeInterval(), 10, 2000));
			}
		);
		menu->addAction("Set Scans To Display Step",
			[this]() {
				auto dialog = new QInputDialog(this);
				dataController_->setScansToDisplayStep(dialog->getInt(this, "Setting Scans To Display Step", "Scans To Display Step[-, <1, 512>]", dataController_->scansToDisplayStep(), 1, 512));
			}
		);
		menu->addAction("Load data from file",
			[this]() {
				auto selectedDeviceType = QInputDialog::getItem(this, "Select device type the signal is comming from", "Type", { "6111", "6132" }, 0, false);
				if(selectedDeviceType == "6111")
					dataController_->loadDataFromFile<Scan6111>(QFileDialog::getOpenFileName(this, "Signal Data File"), selectedDeviceType);
				else if (selectedDeviceType == "6132")
					dataController_->loadDataFromFile<Scan6132>(QFileDialog::getOpenFileName(this, "Signal Data File"), selectedDeviceType);
						
			}
		);
		menu->addAction("Set Maximum Signal Length",
			[this]() {
				auto dialog = new QInputDialog(this);
				dataController_->setMaximumDynamicSignalLength(dialog->getInt(this, "Setting Maximum Dynamic Signal Length", "Maximum Dynamic Signal Length[-, <1, 50000>]", dataController_->maximumDynamicSignalLength(), 1, 50000));
			}
		);

		auto toolButton = new QToolButton(this);
		toolButton->setMenu(menu);
		toolButton->setText(menu->title());
		toolButton->setPopupMode(QToolButton::InstantPopup);

		auto toolButtonAction = new QWidgetAction(this);
		toolButtonAction->setDefaultWidget(toolButton);

		auto toolBar = new QToolBar(this);
		toolBar->addAction(toolButtonAction);
		return toolBar;
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
		layout->setMenuBar(createToolBar());
		layout->addWidget(splitter);
		setLayout(layout);
	}
signals:
};