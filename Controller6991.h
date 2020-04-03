#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStateMachine>
#include <QTime>
#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>

#include "Device6111.h"
#include "AcquisitionStopModeView.h"
#include "AcquisitionStartModeView.h"
#include "TwoStateButton.h"
#include "Defines6991.h"
#include "ScanRateView.h"
#include "StatusView.h"

class Controller6991 : public QGroupBox {
	//TODO
	// Data Storage implementation
	Q_OBJECT
	QStateMachine sm_;
	QTime time_;
	unsigned int id_;
	QWidget* modeGroup_ = new QGroupBox("ControlMode");
	QComboBox* comboBoxMode_ = new QComboBox;
	QPushButton* setModeButton_ = new QPushButton("Set");
	QWidget* dataStreamGroup_ = new QGroupBox("Data Stream");
	QComboBox* dataStreamComboBox_ = new QComboBox;
	TwoStateButton* connectDisconnectButton_ = new TwoStateButton("Connect", [this]() { emit deviceIF_->connectDataStreamRequest(dataStreamComboBox_->currentText().toUInt()); }, "Disconnect", [this]() { emit deviceIF_->disconnectDataStreamRequest(dataStreamComboBox_->currentText().toUInt()); });
	TwoStateButton* acqStartStopButton_ = new TwoStateButton("Start", [this]() { deviceIF_->configurateDeviceRequest(model()); emit deviceIF_->startAcquisitionRequest(); }, "Stop", [this]() { emit deviceIF_->stopAcquisitionRequest(); });
	ScanRateView* scanRateView_ = new ScanRateView;
	QWidget* clockSourceGroup_ = new QGroupBox("Clock Source");
	QComboBox* clockSourceComboBox = new QComboBox;
	AcquisitionStartModeView* startModeView_ = new AcquisitionStartModeView;
	AcquisitionStopModeView* stopModeView_ = new AcquisitionStopModeView;
	QPushButton* resfreshButton_ = new QPushButton("Refresh");
	QPushButton* stopButton_ = new QPushButton("Stop");
	QPushButton* startButton_ = new QPushButton("Start");
	QCheckBox* dataStorageCheckBox_ = new QCheckBox("Data Storage");
	QCheckBox* statusAutoRefreshCheckBox_ = new QCheckBox("Auto Status Refresh");
	StatusView* statusView_ = new StatusView;
	Device6991* deviceIF_;

	QStringList streams = { "1", "2", "3", "4" };
private:
	void createConnections() noexcept;
	void initializeStateMachine() noexcept;
	Configuration6991 model() const noexcept;
private slots:
	void setModel(Configuration6991 const& model) noexcept;
public:
	Controller6991(AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, unsigned int const id, QWidget* parent = nullptr);
};