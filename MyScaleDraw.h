#pragma once
#include "qwt_scale_draw.h"
#include <QString>
#include <map>

class MyPlot;

class MyScaleDraw : public QwtScaleDraw {
	MyPlot* plot_;
public:
	MyScaleDraw(MyPlot* plot);
	virtual ~MyScaleDraw() = default;

	void refresh() noexcept;
	QwtText label(double value) const override;
};