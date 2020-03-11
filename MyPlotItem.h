#pragma once
#include <QObject>
#include <QColor>
#include <qwt_plot_item.h>
#include "Defines.h"
class MyPlot;

class MyPlotItem {
	inline static const QColor selectedColor = Qt::GlobalColor::cyan;
protected:
	bool selected_ = false;
	QColor mainColor_ = Qt::GlobalColor::green;
	MyPlot* plot_;
	bool isPositionedExclusive_ = false;
	MyPlotItem* root_ = this;
public:
	MyPlotItem(MyPlot* plot, MyPlotItem* parent = nullptr);
	virtual ~MyPlotItem() {}

	virtual void move(const double distanceX, const double distanceY) noexcept = 0;
	virtual void setColor(const QColor color) noexcept = 0;
	virtual QRectF boundingRect() const noexcept = 0;
	virtual bool contains(const QPointF& point) const noexcept;
	virtual bool intersects(const QRectF& point) const noexcept;
	bool isRoot() const noexcept {
		return root_ == this;
	}

	MyPlotItem* root() const noexcept {
		return root_;
	}

	void deselect() noexcept;
	bool isPositionedExclusive() const noexcept;
	bool isSelected() const noexcept;
	void select() noexcept;
	bool trySelect(const QPointF& point) noexcept;
	bool trySelect(const QRectF& rect) noexcept;
};
