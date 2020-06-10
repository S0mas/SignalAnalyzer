#pragma once

#include <qwt_plot_picker.h>
#include <optional>

class MyPlot;

class MyPlotPicker : public QwtPlotPicker {
	MyPlot* plot_;
public:
	MyPlotPicker(MyPlot* plot, QWidget* canvas);
	void reset();
	std::optional<double> valueOfCurveUnderCursor(uint32_t const x, int32_t const y) const noexcept;
protected:
	QwtText trackerTextF(QPointF const& pos) const override;
};