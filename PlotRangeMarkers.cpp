#include "MyPlot.h"
#include "Defines.h"
#include "PlotRangeMarkers.h"

PlotMarkerPair::PlotMarkerPair(const double end, PlotMarkerPair* startPartner, MyPlot* plot) 
	: MyPlotMarker(end, plot),
	partner_(startPartner),
	range_([this]() {return partner_->xValue(); }, [this]() {return xValue(); }) {}

PlotMarkerPair::PlotMarkerPair(const double start, const double end, MyPlot* plot)
	: MyPlotMarker(start, plot),
	partner_(new PlotMarkerPair(end, this, plot)),
	range_([this]() {return xValue(); }, [this]() {return partner_->xValue(); }) {
	attach(plot);
	partner_->attach(plot);
	
}

void PlotMarkerPair::remove() noexcept {
	detach();
	partner_->detach();
}
