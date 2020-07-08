#include "View.h"

void View::openCurveBuilderDialog() {
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

SignalCurveType View::curveTypeToBeCreated(CurveData const& data) {
	if (data.vectorize)
		return SignalCurveType::ComplexSignal_Block;
	return data.deviceType_ == DeviceType::_6111 ? SignalCurveType::SingleBitSignal : SignalCurveType::ComplexSignal_Wave;
}

void View::addCurve(QString const& deviceId, const CurveData& data) {
	SignalCurveType type = curveTypeToBeCreated(data);
	for (auto const index : data.channelsSelected) {
		auto curveName = QString("%1 %2").arg(data.nameId_).arg(index);
		data.isRealTimeDataSource_ ? plot_->addDynamicCurve(curveName, type, dataController_->createDataEmitter(deviceId, index))
			: plot_->addStaticCurve(curveName, type, dataController_->staticData(deviceId, index, data.samplesNo_, data.firstSampleId_));
	}
}
