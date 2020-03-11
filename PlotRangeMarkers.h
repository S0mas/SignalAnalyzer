#pragma once
#include "MyPlotMarker.h"
#include <QDebug>

class MyPlot;
class Range;
class QwtPlot;

class PlotMarkerPair : public MyPlotMarker {
	MyPlotMarker* partner_;
	Range range_;
	PlotMarkerPair(const double end, PlotMarkerPair* startPartner, MyPlot* plot);
public:
	PlotMarkerPair(const double start, const double end, MyPlot* plot);
	virtual ~PlotMarkerPair() {
		checkDistance();
		delete partner_;	
	}

	void checkDistance() {
		if(partner_)
			qDebug() << range_.length();
	}
};