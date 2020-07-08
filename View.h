#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>
#include <QVBoxLayout>
#include <QSplitter>
#include <QInputDialog>

#include <memory>
#include <vector>
#include "MyPlot.h"
#include "CurveBuilderDialog.h"
#include "MyPlotCurve.h"
#include "MyPlotIntervalCurve.h"
#include "DataController.h"
#include "SettingsToolbar.h"
#include "AttributeExplorer.h"

class View : public QWidget {
	Q_OBJECT
	MyPlot* plot_ = new MyPlot(this);
	AttributeExplorer* attributeExplorer_ = new AttributeExplorer(plot_->itemsContainer(), this);
	DataController* dataController_ = new DataController(this);
	SettingsToolbar* settingsToolBar_ = new SettingsToolbar(plot_, dataController_);
	SignalCurveType curveTypeToBeCreated(CurveData const& data);
private slots:
	void openCurveBuilderDialog();
	void addCurve(QString const& deviceId, const CurveData& data);
public:
	View(QWidget* parent = nullptr);
signals:
};