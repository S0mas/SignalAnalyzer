#pragma once
#include "MyPlotMarker.h"
#include <QDebug>

class MyPlot;
class Range;
class QwtPlot;

class PlotMarkerPair : public MyPlotMarker {
	PlotMarkerPair* partner_;
	Range range_;
	PlotMarkerPair(const double end, PlotMarkerPair* startPartner, MyPlot* plot);
public:
	PlotMarkerPair(const double start, const double end, MyPlot* plot);
	virtual ~PlotMarkerPair() {
		qDebug() << "pari deleted";
		if (partner_) {
			partner_->partner_ = nullptr;
			partner_->detach();
			delete partner_;
		}		
	}
	void remove() noexcept override;
};