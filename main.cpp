//#include "sfp6100.h"
//#include "../../../TestWizardFramework/Common/include/Device6111.h"
#include <QApplication>
#include <QDebug>
#include "View.h"
#include "CurveBuilderDialog.h"
#include "CurvesPositioner.h"
#include "../WizardFramework/Common/include/Device6991/gui/TestsView.h"
#include "../WizardFramework/Common/include/Device6991/gui/Controller6991.h"
#include "../WizardFramework/Common/include/Device6991/gui/RegisterController.h"
#include "../WizardFramework/Common/include/Device6991/Device6991.h"
#include "../WizardFramework/Mock/include/HardwareConnectorMock.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Controller c;
	View w; 
	QObject::connect(&w, &View::toogleChannel, &c, &Controller::toogleChannel);
	QObject::connect(&w, &View::toogleChannelsGroup, &c, &Controller::toogleChannelsGroup);
	QObject::connect(&w, &View::toogleAllChannels, &c, &Controller::toogleAllChannels);
	QObject::connect(&w, &View::startAcquisition, &c, &Controller::startAcquisition);
	QObject::connect(&w, &View::stopAcquisition, &c, &Controller::stopAcquisition);
	QObject::connect(&w, &View::setDlLinkClockFrequency, &c, &Controller::setDlLinkClockFrequency);
	QObject::connect(&c, &Controller::acquisitionStarted, &w, &View::acquisitionStarted);
	QObject::connect(&c, &Controller::acquisitionStopped, &w, &View::acquisitionStopped);

	//w.show();

	auto device = new AbstractDeviceXX;
	auto device2 = new AbstractDeviceXX;

	auto connector1 = new HardwareConnectorMock("Mock Connector");
	auto connector2 = new HardwareConnectorMock("Mock Connector");
	auto scpiIF = new ScpiIFMock;

	auto controller6991 = new Controller6991(connector1, scpiIF, 1);
	auto controller6991_2 = new Controller6991(connector2, scpiIF, 2);
	auto resgisterController = new RegisterController(device, device2);
	auto testController = new TestsView(device);

	controller6991->show();
	controller6991_2->show();
	resgisterController->show();
	testController->show();
	controller6991->show();

	return a.exec();
}
