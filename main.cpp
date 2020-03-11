#include "sfp6100.h"
#include "../../../TestWizardFramework/Common/include/Device6111.h"
#include <QApplication>
#include <QDebug>
#include "View.h"
#include "CurveBuilderDialog.h"
#include "CurvesPositioner.h"

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
	Device6111 dev("Eloszka");
	qDebug() << dev.channels().size();
	qDebug() << dev.nameId();
	w.show();
	Positioner<QString> p;
	QString s1 = "A";
	QString s2 = "B";
	QString s3 = "C";
	QString s4 = "D";
	QString s5 = "E";

	return a.exec();
}
