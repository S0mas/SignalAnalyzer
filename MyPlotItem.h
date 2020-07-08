#pragma once
#include <QObject>
#include <QColor>
#include <qwt_plot_item.h>
#include <optional>
#include "Defines.h"
#include "DataEmitter.h"
#include <QStandardItem>

class MyPlot;

class MyPlotItem : public QObject {
	Q_OBJECT
	inline static const QColor selectedColor = Qt::GlobalColor::cyan;
protected:
	Q_PROPERTY(QString nameId MEMBER nameId_)
	Q_PROPERTY(bool selected MEMBER selected_ NOTIFY selectedChanged)
	bool selected_ = false;
	QColor mainColor_ = Qt::GlobalColor::green;
	MyPlot* plot_;
	bool isPositionedExclusive_ = false;
	MyPlotItem* root_ = this;
	QString nameId_;
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

	bool isPositionedExclusive() const noexcept;
	bool isSelected() const noexcept;
	bool trySelect(const QPointF& point) noexcept;
	bool trySelect(const QRectF& rect) noexcept;
	virtual bool isCurve() const noexcept { return false; }
public slots:
	void updateColorAfterSelectionChanged(bool const selected);
signals:
	void selectedChanged(bool const selected) const;
};

class MyPlotAbstractCurve : public MyPlotItem {
	Q_OBJECT
	bool isRealTimeCurve_;
public:
	MyPlotAbstractCurve(MyPlot* plot, bool const isRealTimeCurve = true, MyPlotItem* parent = nullptr) : MyPlotItem(plot, parent), isRealTimeCurve_(isRealTimeCurve) {}
	void connectDataEmitter(DataEmitter* dataEmitter) noexcept {
		connect(this, &MyPlotAbstractCurve::dataRequest, dataEmitter, &DataEmitter::handleDataRequest);
		connect(dataEmitter, &DataEmitter::data, this, &MyPlotAbstractCurve::handleData);
	}
	~MyPlotAbstractCurve() override = default;
	virtual bool isVisibleOnScreen() const noexcept = 0;
	bool isRealTimeCurve() const noexcept {
		return isRealTimeCurve_;
	}

	virtual std::optional<double> value(double const x) const noexcept = 0;
	virtual void setScaleManual(double const min, double const max) noexcept {}
	virtual void setScaleAuto() noexcept {}
	bool isCurve() const noexcept override { return true; }
public slots:
	virtual void handleData(std::pair<std::vector<double>, std::vector<Timestamp6991>> const& data) = 0;
signals:
	void dataRequest() const;
};