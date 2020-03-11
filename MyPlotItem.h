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
public:
	MyPlotItem(MyPlot* plot);
	virtual ~MyPlotItem() = default;

	virtual void move(const QPointF& destination) noexcept = 0;
	virtual void setColor(const QColor color) noexcept = 0;
	virtual QRectF boundingRect() const noexcept = 0;
	virtual void remove() noexcept = 0;
	virtual bool contains(const QPointF& point) const noexcept;
	virtual bool intersects(const QRectF& point) const noexcept;

	void deselect() noexcept;
	bool isPositionedExclusive() const noexcept;
	bool isSelected() const noexcept;
	void select() noexcept;
	bool trySelect(const QPointF& point) noexcept;
	bool trySelect(const QRectF& rect) noexcept;
};
