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
#include <QRandomGenerator>
#include <functional>
#include <vector>

#include "MyPlot.h"
#include "MyIntervalSymbol.h"
#include "MyPlotIntervalCurve.h"
#include "MyScaleDraw.h"
#include "MyPlotMagnifier.h"
#include "MyPlotPanner.h"
#include "MyCurve.h"

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
public:
	PlotView(QWidget* parent = nullptr) : QGroupBox("Plot", parent) {
		plot = new MyPlot(this);
		connect(plot, &MyPlot::addCurveActionStarted, this, &PlotView::addCurveActionStarted);
		auto layout = new QVBoxLayout();
		layout->addWidget(plot);
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