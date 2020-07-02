#pragma once
#include <QToolBar>

class MyPlot;
class DataController;
class SettingsToolbar : public QToolBar {
	Q_OBJECT
private slots:
	void setMaximumSignalLength();
	void loadDataFromFile();
	void setScanToDisplayStep();
	void setRefreshTimeInterval();
public:
	MyPlot* plot_;
	DataController* dataController_;
	SettingsToolbar(MyPlot*, DataController*);
};