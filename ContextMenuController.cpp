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
	menu->addAction("Remove", plot(), SLOT(removeItems()));
	menu->addAction("Set Scale", plot(), SLOT(setScale()));
	menu->popup(QCursor::pos());
}

void ContextMenuController::contextMenuMulit() {
	QMenu* menu = new QMenu(plot());
	menu->addAction("Remove", plot(), SLOT(removeItems()));
	menu->addAction("Overlap", plot(), SLOT(overlap()));
	menu->addAction("Separate", plot(), SLOT(separate()));
	menu->addAction("Set Scale", plot(), SLOT(setScale()));
	//menu->addAction("Vectorize", plot(), SLOT(separate()));
	//menu->addAction("Explode", plot(), SLOT(separate()));
	menu->popup(QCursor::pos());
}

void ContextMenuController::contextMenuEmpty() {
	QMenu* menu = new QMenu(plot());
	menu->addAction("Add Curve", plot(), SIGNAL(addCurveActionStarted()));
	menu->addAction("Add Marker", plot(), SLOT(addMarkerAction()));
	menu->addAction("Add Range Markers", plot(), SLOT(addRangeMarkersAction()));
	menu->addAction("Remove All", plot(), SLOT(removeAll()));
	menu->popup(QCursor::pos());
}