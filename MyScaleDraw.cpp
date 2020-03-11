#include "MyScaleDraw.h"
#include "MyPlot.h"

MyScaleDraw::MyScaleDraw(MyPlot* plot) : QwtScaleDraw(), plot_(plot){
	setAlignment(LeftScale);
	setTickLength(QwtScaleDiv::MediumTick, 0);
	setTickLength(QwtScaleDiv::MinorTick, 0);
}

void MyScaleDraw::refresh() noexcept {
	invalidateCache();
}


QwtText MyScaleDraw::label(double value) const {
	auto curve = plot_->positioner().curve(value);
	return curve ? QString("%1").arg(dynamic_cast<MyPlotIntervalCurve*>(curve)->nameId()) : "";
}
