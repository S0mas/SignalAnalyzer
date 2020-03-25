//#include "sfp6100.h"
//#include "../../../TestWizardFramework/Common/include/Device6111.h"
#include <QApplication>
#include <QDebug>
#include "View.h"
#include "CurveBuilderDialog.h"
#include "CurvesPositioner.h"
#include "AutoTestController.h"
#include "Controller6991.h"
#include "RegisterController.h"

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

	w.show();

	//auto device = new AbstractDevice;
	//auto device2 = new AbstractDevice;

	//auto controller6991 = new Controller6991(device, 1);
	//auto resgisterController = new RegisterController(device, device2);
	//auto testController = new AutoTestController(device);

	//controller6991->show();
	//resgisterController->show();
	//testController->show();
	//controller6991->show();

	return a.exec();
}
