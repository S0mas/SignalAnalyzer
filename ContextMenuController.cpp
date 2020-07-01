#include "ContextMenuController.h"
#include "MyPlot.h"

#include <QAction>
#include <QEvent>
#include <QMenu>

#include <qwt_plot_canvas.h>

ContextMenuController::ContextMenuController(MyPlot* plot) : QObject(plot) {
	QwtPlotCanvas* canvas = dynamic_cast<QwtPlotCanvas*>(plot->canvas());
	canvas->installEventFilter(this);
	canvas->setFocusPolicy(Qt::StrongFocus);
	canvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
}

bool ContextMenuController::eventFilter(QObject* object, QEvent* event) noexcept {
	if (plot() == NULL || object != plot()->canvas())
		return false;
	switch (event->type()) {
	case QEvent::MouseButtonPress:
		switch (static_cast<QMouseEvent*>(event)->button()) {
		case Qt::MouseButton::RightButton:
			if (plot()->isMultipleObjectsSelected())
				contextMenuMulit();
			else if (plot()->isSomethingSelected())
				contextMenuSingle();
			else
				contextMenuEmpty();
			break;
		default:
			break;
		}
	default:
		break;
	}
	return QObject::eventFilter(object, event);
}

MyPlot* ContextMenuController::plot() noexcept {
	return dynamic_cast<MyPlot*>(parent());
}

const MyPlot* ContextMenuController::plot() const noexcept {
	return dynamic_cast<const MyPlot*>(parent());
}

void ContextMenuController::contextMenuSingle() {
	QMenu* menu = new QMenu(plot());
	menu->addAction("Remove", plot(),"removeItems");
	menu->popup(QCursor::pos());
}

void ContextMenuController::contextMenuMulit() {
	QMenu* menu = new QMenu(plot());
	menu->addAction("Remove", plot(), "removeItems");
	menu->popup(QCursor::pos());
}

void ContextMenuController::contextMenuEmpty() {
	QMenu* menu = new QMenu(plot());
	menu->addAction("Add Curve", plot(), "addCurveActionStarted");
	menu->addAction("Add Marker", plot(),"addMarkerAction");
	menu->addAction("Add Range Markers", plot(), "addRangeMarkersAction");
	menu->popup(QCursor::pos());
}