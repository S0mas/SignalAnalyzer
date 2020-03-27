#pragma once
#include <QCheckBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QVector>
#include <QColumnView>
#include <QRandomGenerator>
#include <functional>
#include <QSplitter>
#include <vector>

#include "MyPlot.h"
#include "MyIntervalSymbol.h"
#include "MyPlotIntervalCurve.h"
#include "MyScaleDraw.h"
#include "MyPlotMagnifier.h"
#include "MyPlotPanner.h"

#include "CanvasSelector.h"
#include "ScalePicker.h"
#include <qevent.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_scale_widget.h>
#include <qwt_wheel.h>
#include <stdlib.h>

#include <qwt_plot.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_interval_symbol.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_engine.h>

class PlotView : public QGroupBox {
	Q_OBJECT
	MyPlot* plot;
	QColumnView* propertiesColumnView() {
		QColumnView *cview = new QColumnView;

		/* Create the data model */
		auto model = new QStandardItemModel;

		for (int groupnum = 0; groupnum < 3; ++groupnum) {
			/* Create the phone groups as QStandardItems */
			QStandardItem *group = new QStandardItem(QString("Group %1").arg(groupnum));

			/* Append to each group 5 person as children */
			for (int personnum = 0; personnum < 5; ++personnum) {
				QStandardItem *child = new QStandardItem(QString("Person %1 (group %2)").arg(personnum).arg(groupnum));
				/* the appendRow function appends the child as new row */
				group->appendRow(child);
			}
			/* append group as new row to the model. model takes the ownership of the item */
			model->appendRow(group);
		}

		cview->setModel(model);

		return cview;
	}
public:
	PlotView(QWidget* parent = nullptr) : QGroupBox("Plot", parent) {
		plot = new MyPlot(this);
		connect(plot, &MyPlot::addCurveActionStarted, this, &PlotView::addCurveActionStarted);

		auto splitter = new QSplitter(Qt::Orientation::Horizontal);
		splitter->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
		plot->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
		auto propertiesColumn = propertiesColumnView();
		propertiesColumn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		splitter->addWidget(plot);
		splitter->addWidget(propertiesColumn);
		splitter->setContentsMargins(0, 0, 0, 0);
		
		auto layout = new QHBoxLayout();
		layout->setSpacing(0);
		layout->addWidget(splitter);
		layout->setMargin(0);
		layout->setContentsMargins(0, 0, 0, 0);
		setLayout(layout);
	}
public slots:
	void addCurveSingle(const QString& nameId, const DataGetterFunction& dataGetter) {
		plot->addCurve(nameId, dataGetter, 1);
	}

	void addCurveVec(const QString& nameId, const DataGetterFunction& dataGetter) {
		plot->addCurve(nameId, dataGetter, 0);
	}
signals:
	void addCurveActionStarted() const;
};