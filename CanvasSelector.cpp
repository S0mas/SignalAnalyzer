#include "CanvasSelector.h"
#include "Defines.h"
#include "MyPickerDragRectMachine.h"
#include "MyPlot.h"
#include "MyPlotPicker.h"
#include <cmath>

#include <QEvent>
#include <QRectF>
#include <QDebug>

#include <qwt_plot_canvas.h>

CanvasSelector::CanvasSelector(MyPlot* plot) : QObject(plot) {
	auto canvas = dynamic_cast<QwtPlotCanvas*>(plot->canvas());
	canvas->installEventFilter(this);
	canvas->setFocusPolicy(Qt::StrongFocus);
	canvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);

	picker_ = new MyPlotPicker(plot, canvas);
	picker_->setStateMachine(new MyPickerDragRectMachine());
	picker_->setRubberBandPen(QColor(Qt::cyan));
	picker_->setTrackerPen(QColor(Qt::green));
	picker_->setMousePattern({ QwtEventPattern::MousePattern(Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier), QwtEventPattern::MousePattern(Qt::MouseButton::LeftButton, Qt::KeyboardModifier::ControlModifier) });
	connect(picker_, SIGNAL(selected(const QRectF&)), this, SLOT(select(const QRectF&)));
}

bool CanvasSelector::eventFilter(QObject* object, QEvent* event) noexcept {
	if (!plot() || object != plot()->canvas())
		return false;
	switch (event->type()) {
	case QEvent::MouseButtonPress:
		switch (static_cast<QMouseEvent*>(event)->button()) {
		case Qt::MouseButton::LeftButton:
			if (!isControlButtonHold())
				if (!isContainedBySelection(plot()->cursorPosition()))
					deselect();
				else {
					picker_->reset();
					return true;
				}			
			break;
		default:
			break;
		}
	defualt:
		break;
	}
	return QObject::eventFilter(object, event);
}

void CanvasSelector::deselect() const noexcept {
	plot()->deselect();
}

bool CanvasSelector::isContainedBySelection(const QPoint& point) const noexcept {
	auto result = false;
	QPointF valuePoint = plot()->plotPointToValuePoint(point);
	plot()->forEachMyPlotItem([valuePoint, &result](auto ptr) { if(ptr->isSelected()) result = ptr->contains(valuePoint); }, [&result]() {return !result; });
	return result;
}

MyPlot* CanvasSelector::plot() const noexcept {
	return dynamic_cast<MyPlot*>(parent());
}

void CanvasSelector::select(const QRectF& rect) const noexcept {
	if (rect.bottomLeft() == rect.topRight()) {
		plot()->forEachMyPlotItem([rect](auto ptr) { ptr->trySelect(rect.bottomLeft()); });
		picker_->reset();
	}
	else
		plot()->forEachMyPlotItem([rect](auto ptr) { ptr->trySelect(rect); });
	plot()->updatePlot();
}