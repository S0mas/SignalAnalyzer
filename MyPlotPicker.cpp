#include "MyPlotPicker.h"
#include "MyPlot.h"

MyPlotPicker::MyPlotPicker(MyPlot * plot, QWidget * canvas) : QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn, canvas), plot_(plot) {}

void MyPlotPicker::reset() {
	QwtPlotPicker::reset();
}

std::optional<double> MyPlotPicker::valueOfCurveUnderCursor(uint32_t const x, int32_t const y) const noexcept {
	auto curve = plot_->findCurveByPosition(y);
	return curve ? curve->value(x) : std::nullopt;
}

QwtText MyPlotPicker::trackerTextF(QPointF const& pos) const {
	QString text;
	uint32_t x = floor(pos.x());
	uint32_t y = floor(pos.y());

	if (auto value = valueOfCurveUnderCursor(x, y))
		text.sprintf("%d, %.4f", x, *value);
	else
		text.sprintf("%d, no data", x);
	return QwtText(text);
}
