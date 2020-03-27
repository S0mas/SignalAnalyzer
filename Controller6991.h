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

#include "AbstractDevice.h"
#include "AcquisitionStopModeView.h"
#include "AcquisitionStartModeView.h"
#include "TwoStateButton.h"
#include "Defines.h"
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
	TwoStateButton* connectDisconnectButton_ = new TwoStateButton("Connect", [this]() { emit connectReq(id_, dataStreamComboBox_->currentText().toUInt()); }, "Disconnect", [this]() { emit disconnectReq(id_); });
	TwoStateButton* acqStartStopButton_ = new TwoStateButton("Start", [this]() { emit startAcqReq(id_, model()); }, "Stop",  [this]() { emit stopAcqReq(id_); });
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

	QStringList streams = { "1", "2", "3", "4" };
signals:
	void changeControlModeReq(unsigned int id_, ControlMode const mode) const;
	void changeToController() const;
	void changeToListener() const;
	void connectReq(int const id, int const port) const;
	void disconnectReq(int const id) const;
	void connected() const;
	void statusReq(int const id) const;
	void startAcqReq(int const id, AcquisitionConfigurationDataModel const& configModel);
	void stopAcqReq(int const id);
private:
	void createConnections(AbstractDevice* device) noexcept;
	void initializeStateMachine(AbstractDevice* device) noexcept;
	AcquisitionConfigurationDataModel model() const noexcept;
private slots:
	void setModel(AcquisitionConfigurationDataModel const& model) noexcept;
public:
	Controller6991(AbstractDevice* device, unsigned int const id, QWidget* parent = nullptr);
};