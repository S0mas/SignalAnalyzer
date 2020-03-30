#pragma once
#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDebug>

#include "AbstractDevice.h"
#include "Command.h"
#include "CommandTypes.h"
#include "CustomTransitions.h"
#include "EnumSelector.h"
#include "RegisterTypes.h"

class TargetFrontendCardView : public QGroupBox {
	Q_OBJECT
	QLineEdit* lineEdit_ = new QLineEdit;
	QCheckBox* checkBox_ = new QCheckBox;
	QStateMachine sm_;
	void createConnections(AbstractDevice* device) noexcept;
	void initializeStateMachine(AbstractDevice* device) noexcept;
signals:
	void commandExecutionReq(Command const cmd) const;
public:
	TargetFrontendCardView(AbstractDevice* device, int const index, QWidget* parent = nullptr);
signals:
	void sendCommand(unsigned int const cmd, unsigned int const address) const;
	void stateReq() const;
};

class RegisterController : public QGroupBox {
	Q_OBJECT
	EnumSelector* commandSelector_ = new EnumSelector(std::make_unique<CommandTypes>(), "Command");
	EnumSelector* addressSelector_ = new EnumSelector(std::make_unique<RegistersTypes>(), "Address");
	TargetFrontendCardView* frontend1_;
	TargetFrontendCardView* frontend2_;
	QPushButton* refreshButton = new QPushButton("Refresh");
	QPushButton* executeButton = new QPushButton("Execute");
	void createConnections(AbstractDevice* device1, AbstractDevice* device2) noexcept;
public:
	RegisterController(AbstractDevice* device1, AbstractDevice* device2, QWidget* parent = nullptr);
};