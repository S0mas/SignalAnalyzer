#include "CanvasManipulator.h"
#include "Defines.h"
#include "MyPlot.h"
#include "MyPlotItem.h"
#include "MyPlotPicker.h"

#include <cmath>
#include <vector>

#include <QEvent>
#include <QRectF>
#include <QDebug>

#include <qwt_plot_canvas.h>

CanvasManipulator::CanvasManipulator(MyPlot* plot) : QObject(plot) {
	auto canvas = dynamic_cast<QwtPlotCanvas*>(plot->canvas());
	canvas->installEventFilter(this);
	canvas->setFocusPolicy(Qt::StrongFocus);
	canvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);

	picker_ = new MyPlotPicker(canvas);
	picker_->setStateMachine(new MyPickerDragRectMachine());
	picker_->setRubberBandPen(QColor(Qt::cyan));
	picker_->setTrackerPen(QColor(Qt::green));
	picker_->setMousePattern({ QwtEventPattern::MousePattern(Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier), QwtEventPattern::MousePattern(Qt::MouseButton::LeftButton, Qt::KeyboardModifier::ControlModifier) });
	connect(picker_, SIGNAL(selected(const QRectF&)), this, SLOT(select(const QRectF&)));
}

bool CanvasManipulator::eventFilter(QObject* object, QEvent* event) noexcept {
	if (!plot() || object != plot()->canvas())
		return false;
	switch (event->type()) {
	case QEvent::MouseButtonPress:
		switch (static_cast<QMouseEvent*>(event)->button()) {
		case Qt::MouseButton::LeftButton:
			//if(state_ == STATE::NO_SELECTION)
			if (!isControlButtonHold())
				if (!isContainedBySelection(plot()->cursorPosition()))
					deselect();
				else {
					moveStart_ = plot()->cursorPosition();
					picker_->reset();
					return true;
				}
			break;
		default:
			break;
		}
	case QEvent::MouseMove:
		if (isMouseLeftButtonHold(event) && !isControlButtonHold() && plot()->isSomethingSelected()) {
			move(plot()->selection(), plot()->cursorPosition());
			return true;
		}
		break;
	defualt:
		break;
	}
	return QObject::eventFilter(object, event);
}

void CanvasManipulator::deselect() const noexcept {
	plot()->deselect();
}

bool CanvasManipulator::isContainedBySelection(const QPoint& point) const noexcept {
	auto result = false;
	QPointF valuePoint = plot()->plotPointToValuePoint(point);
	plot()->forEachMyPlotItem([valuePoint, &result](auto ptr) { if (ptr->isSelected()) result = ptr->contains(valuePoint); }, [&result]() {return !result; });
	return result;
}

MyPlot* CanvasManipulator::plot() const noexcept {
	return dynamic_cast<MyPlot*>(parent());
}

void CanvasManipulator::select(const QRectF& rect) const noexcept { 
	if (rect.bottomLeft() == rect.topRight()) {
		plot()->forEachMyPlotItem([rect](auto ptr) { ptr->trySelect(rect.bottomLeft()); });
		picker_->reset();
	}
	else
		plot()->forEachMyPlotItem([rect](auto ptr) { ptr->trySelect(rect); });
	plot()->updatePlot();
}

void CanvasManipulator::move(std::vector<MyPlotItem*>& items, const QPoint& pos) noexcept {
	if (items.empty())
		return;

	auto destinationValuePoint = plot()->plotPointToValuePoint(pos);
	auto moveStartValuePoint = plot()->plotPointToValuePoint(moveStart_);
	auto distanceX = destinationValuePoint.x() - moveStartValuePoint.x();
	auto distanceY = destinationValuePoint.y() - moveStartValuePoint.y();

	for (auto item : items)
		if (!item->isPositionedExclusive())
			item->move(distanceX, distanceY);
	moveStart_ = pos;
	plot()->positioner().move(items, destinationValuePoint);
	plot()->updatePlot();
}