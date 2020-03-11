#pragma once

#include <qwt_plot_panner.h>
#include <qwt_plot.h>

class MyPlotPanner final : public QwtPlotPanner {
protected:
	void moveCanvas(int dx, int dy) override {
		if (dx == 0 && dy == 0)
			return;

		QwtPlot* plot = this->plot();
		if (plot == NULL)
			return;

		const bool doAutoReplot = plot->autoReplot();
		plot->setAutoReplot(false);

		for (int axis = 0; axis < QwtPlot::axisCnt; axis++) {
			if (!isAxisEnabled(axis))
				continue;

			const QwtScaleMap map = plot->canvasMap(axis);

			const double p1 = map.transform(plot->axisScaleDiv(axis).lowerBound());
			const double p2 = map.transform(plot->axisScaleDiv(axis).upperBound());

			double d1, d2;
			if (axis == QwtPlot::xBottom || axis == QwtPlot::xTop) {
				d1 = map.invTransform(p1 - dx);
				d2 = map.invTransform(p2 - dx);
			}
			else {
				d1 = map.invTransform(p1 - dy);
				d2 = map.invTransform(p2 - dy);
			}
			if(axis == QwtPlot::yLeft)
				plot->setAxisScale(axis, d1, d2, 1);
			else
				plot->setAxisScale(axis, d1, d2);
		}

		plot->setAutoReplot(doAutoReplot);
		plot->replot();
	}
public:
	MyPlotPanner(QWidget* parent = nullptr) : QwtPlotPanner(parent) {}
};