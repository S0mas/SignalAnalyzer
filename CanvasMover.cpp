#include "CanvasMover.h"
#include "Defines.h"
#include "MyPlot.h"

#include <QEvent>

#include <qwt_plot_canvas.h>

CanvasMover::CanvasMover(MyPlot* plot) : QObject(plot) {
	auto canvas = dynamic_cast<QwtPlotCanvas*>(plot->canvas());
	canvas->installEventFilter(this);
	canvas->setFocusPolicy(Qt::StrongFocus);
	canvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
}

bool CanvasMover::eventFilter(QObject* object, QEvent* event) noexcept {
	if (!plot() || object != plot()->canvas())
		return false;
	switch (event->type()) {
	case QEvent::MouseMove:
		if (isMouseLeftButtonHold(event) && !isControlButtonHold() && plot()->isSomethingSelected()) {
			move(plot()->selection(), plot()->cursorPosition());
			return true;
		}
		break;
	default:
		break;
	}
	return QObject::eventFilter(object, event);
}

MyPlot* CanvasMover::plot() const noexcept {
	return dynamic_cast<MyPlot*>(parent());
}

void CanvasMover::move(std::vector<MyPlotItem*>& items, const QPoint& pos) const noexcept {
	if (items.empty())
		return;
	plot()->positioner().move(items, plot()->plotPointToValuePoint(pos));
	plot()->updatePlot();
}