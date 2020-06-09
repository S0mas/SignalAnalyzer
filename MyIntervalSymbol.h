#pragma once

#include <qwt_interval_symbol.h>
#include <qwt_painter.h>
#include <qwt_math.h>
#include <qpainter.h>
#include <QDebug>
#include <QString>
#include <QGraphicsSimpleTextItem>
#include <QStyleOptionGraphicsItem>

class MyIntervalSymbol : public QwtIntervalSymbol {
public:
	MyIntervalSymbol() : QwtIntervalSymbol(QwtIntervalSymbol::UserSymbol) {}
	void draw(QPainter* painter, Qt::Orientation orientation, const QPointF& from, const QPointF& to, int value, int size, int hexLength = 4, bool hex = true) const {
		const qreal pw = qMax(painter->pen().widthF(), qreal(1.0));
		QPointF p1 = from.toPoint();
		QPointF p2 = to.toPoint();
		if (width() <= pw)
			QwtPainter::drawLine(painter, p1, p2);
		else {
			auto L = p2.x() - p1.x();
			auto b = L / 16;

			QPolygonF polygon;
			polygon += QPointF(p1.x(), p1.y() - size / 2);
			polygon += QPointF(p1.x() + b, p1.y());
			polygon += QPointF(p2.x() - b, p1.y());
			polygon += QPointF(p2.x(), p1.y() - size / 2);
			polygon += QPointF(p2.x() - b, p1.y()-size);
			polygon += QPointF(p1.x() + b, p1.y()-size);

			QwtPainter::drawPolygon(painter, polygon);
			auto str = hex ? QString("0x%1").arg(value, hexLength, hex ? 16 : 10, QLatin1Char('0')) : QString("%1").arg(value);
			auto font = painter->font();
			auto fontSize = size < L ? size / str.size() : L / str.size();
			font.setPointSize(fontSize);
			painter->setFont(font);
			QwtPainter::drawText(painter, polygon.boundingRect(), Qt::AlignCenter, str);
		}
	}
};

class MyIntervalSymbol2 : public QwtIntervalSymbol {
public:
	MyIntervalSymbol2() : QwtIntervalSymbol(QwtIntervalSymbol::UserSymbol) {}
	void draw(QPainter* painter, Qt::Orientation orientation, const QPointF& from, const QPointF& to, bool value, int size, bool addValueChangedBar) const {
		const qreal pw = qMax(painter->pen().widthF(), qreal(1.0));
		QPointF p1 = from.toPoint();
		QPointF p2 = to.toPoint();

		auto pen = painter->pen();
		pen.setWidthF(size/20);
		painter->setPen(pen);
		auto penW = painter->pen().widthF();

		auto y0 = p1.y() - penW;
		auto y1 = p1.y() - size + penW;
		if (addValueChangedBar)
			QwtPainter::drawLine(painter, QPointF(p1.x(), y0), QPointF(p1.x(), y1));

		auto y = value ? y1 : y0;
		QwtPainter::drawLine(painter, QPointF(p1.x(), y), QPointF(p2.x(), y));
	}

	~MyIntervalSymbol2() override = default;
};