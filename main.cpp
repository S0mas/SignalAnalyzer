#include <QApplication>
#include <QDebug>
#include "View.h"
#include "CurveBuilderDialog.h"
#include "CurvesPositioner.h"

int main(int argc, char *argv[]) {
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

	return a.exec();
}
