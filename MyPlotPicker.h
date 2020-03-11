#pragma once
#include <qwt_plot_picker.h>

class MyPlotPicker : public QwtPlotPicker {
public:
	MyPlotPicker(QWidget* canvas) : QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn, canvas) {}
	void reset() {
		QwtPlotPicker::reset();
	}
protected:
	QwtText trackerTextF(const QPointF& pos) const override {
		QString text;
		text.sprintf("%.4f", pos.x());
		return QwtText(text);
	}
};