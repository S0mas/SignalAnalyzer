#include "MyPlot.h"
#include "Defines.h"
#include "PlotRangeMarkers.h"

PlotMarkerPair::PlotMarkerPair(const double start, const double end, MyPlot* plot)
	: MyPlotMarker(start, plot),
	partner_(new MyPlotMarker(end, plot, this)),
	range_([this]() {return xValue(); }, [this]() {return partner_->xValue(); }) {
	attach(plot);
	partner_->attach(plot);
}
