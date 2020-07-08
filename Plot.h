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
#include <QtGlobal>
#include <functional>
#include <QSplitter>
#include <QToolBar>
#include <vector>

#include "MyPlot.h"
#include "MyIntervalSymbol.h"
#include "MyPlotIntervalCurve.h"
#include "MyScaleDraw.h"
#include "MyPlotMagnifier.h"
#include "MyPlotPanner.h"
#include "AttributeExplorer.h"

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

class PlotView : public QWidget {
	Q_OBJECT
	MyPlot* plot;
public:
	PlotView(QWidget* parent = nullptr) : QWidget(parent) {
		plot = new MyPlot(this);
		connect(plot, &MyPlot::addCurveActionStarted, this, &PlotView::addCurveActionStarted);

		auto splitter = new QSplitter(Qt::Orientation::Horizontal);
		splitter->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
		plot->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
		splitter->addWidget(plot);
		auto attributeExplorer = new AttributeExplorer(this);
		splitter->addWidget(attributeExplorer);
		splitter->setContentsMargins(0, 0, 0, 0);

		auto layout = new QHBoxLayout();
		layout->setSpacing(0);
		layout->addWidget(splitter);
		layout->setMargin(0);
		layout->setContentsMargins(0, 0, 0, 0);
		setLayout(layout);
	}

	MyPlot* myPlot() noexcept {
		return plot;
	}

public slots:
	void addSingleBitSignal(const QString& nameId, DataEmitter* dataEmitter) {
		auto curve = plot->addCurve(nameId, SignalCurveType::SingleBitSignal);
		curve->connectDataEmitter(dataEmitter);
	}

	void addComplexSignalWave(const QString& nameId, DataEmitter* dataEmitter) {
		auto curve = plot->addCurve(nameId, SignalCurveType::ComplexSignal_Wave);
		curve->connectDataEmitter(dataEmitter);
	}

	void addComplexSignalBlock(const QString& nameId, DataEmitter* dataEmitter) {
		auto curve = plot->addCurve(nameId, SignalCurveType::ComplexSignal_Block);
		curve->connectDataEmitter(dataEmitter);
	}

	void addStaticSingleBitSignal(const QString& nameId, std::pair<std::vector<double>, std::vector<Timestamp6991>> const& data) {
		auto curve = plot->addCurve(nameId, SignalCurveType::SingleBitSignal, false, data);
	}

	void addStaticComplexSignalWave(const QString& nameId, std::pair<std::vector<double>, std::vector<Timestamp6991>> const& data) {
		auto curve = plot->addCurve(nameId, SignalCurveType::ComplexSignal_Wave, false, data);
	}

	void addStaticComplexSignalBlock(const QString& nameId, std::pair<std::vector<double>, std::vector<Timestamp6991>> const& data) {
		auto curve = plot->addCurve(nameId, SignalCurveType::ComplexSignal_Block, false, data);
	}
signals:
	void addCurveActionStarted() const;
};