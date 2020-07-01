#include "SettingsToolbar.h"
#include "MyPlot.h"
#include "DataController.h"
#include <QFileDialog>
#include <QToolButton>
#include <QWidgetAction>
#include <QInputDialog>
#include <QMenu>

void SettingsToolbar::setMaximumSignalLength() {
	auto dialog = new QInputDialog(this);
	bool pressedOk = false;
	auto input = dialog->getInt(this, "Setting Maximum Dynamic Signal Length", "Maximum Dynamic Signal Length[-, <1, 50000>]", dataController_->maximumDynamicSignalLength(), 1, 50000, 1, &pressedOk);
	if (pressedOk)
		dataController_->setMaximumDynamicSignalLength(input);
}

SettingsToolbar::SettingsToolbar(MyPlot* plot, DataController* dataController) : plot_(plot), dataController_(dataController) {
	auto toolButton = new QToolButton(this);
	auto menu = new QMenu("Settings", this);
	menu->addAction("Set Refresh Time Interval", this, SLOT(setRefreshTimeInterval()));
	menu->addAction("Set Scans To Display Step", this, SLOT(setScanToDisplayStep()));
	menu->addAction("Load data from file", this, SLOT(loadDataFromFile()));
	menu->addAction("Set Maximum Signal Length", this, SLOT(setMaximumSignalLength()));
	toolButton->setMenu(menu);
	toolButton->setText(menu->title());
	toolButton->setPopupMode(QToolButton::InstantPopup);

	auto toolButtonAction = new QWidgetAction(this);
	toolButtonAction->setDefaultWidget(toolButton);
	addAction(toolButtonAction);
}

void SettingsToolbar::loadDataFromFile() {
	bool pressedOk = false;
	auto selectedDeviceType = QInputDialog::getItem(this, "Select device type the signal is comming from", "Type", { "6111", "6132" }, 0, false, &pressedOk);
	if (pressedOk && selectedDeviceType == "6111")
		dataController_->loadDataFromFile<Scan6111>(QFileDialog::getOpenFileName(this, "Signal Data File"), selectedDeviceType);
	else if (pressedOk && selectedDeviceType == "6132")
		dataController_->loadDataFromFile<Scan6132>(QFileDialog::getOpenFileName(this, "Signal Data File"), selectedDeviceType);

}

void SettingsToolbar::setScanToDisplayStep() {
	auto dialog = new QInputDialog(this);
	bool pressedOk = false;
	auto input = dialog->getInt(this, "Setting Scans To Display Step", "Scans To Display Step[-, <1, 512>]", dataController_->scansToDisplayStep(), 1, 512, 1, &pressedOk);
	if (pressedOk)
		dataController_->setScansToDisplayStep(input);
}

void SettingsToolbar::setRefreshTimeInterval() {
	auto dialog = new QInputDialog(this);
	bool pressedOk = false;
	auto input = dialog->getInt(this, "Setting Refresh Time Interval", "Refresh Time Interval[ms, <10, 2000>]", plot_->refreshTimeInterval(), 10, 2000, 1, &pressedOk);
	if (pressedOk)
		plot_->setRefreshTimeInterval(input);
}
