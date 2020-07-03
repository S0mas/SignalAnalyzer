#include "MyPlotPicker.h"
#include "MyPlot.h"

MyPlotPicker::MyPlotPicker(MyPlot * plot, QWidget * canvas) : QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn, canvas), plot_(plot) {}

void MyPlotPicker::reset() {
	QwtPlotPicker::reset();
}

std::optional<double> MyPlotPicker::valueOfCurveUnderCursor(double const x, int32_t const y) const noexcept {
	auto curve = plot_->findCurveByPosition(y);
	return curve ? curve->value(x) : std::nullopt;
}

QwtText MyPlotPicker::trackerTextF(QPointF const& pos) const {
	QString text;
	auto x = pos.x();
	uint32_t y = floor(pos.y());

	if (auto value = valueOfCurveUnderCursor(x, y); value)
		text.sprintf("%.4f, %.4f", x, *value);
	else
		text.sprintf("%.4f, no data", x);
	return QwtText(text);
}
