#pragma once
#include "MyPlotIntervalCurve.h"
#include "PlotRangeMarkers.h"
#include "MyPlotMarker.h"

#include <set>
#include <memory>
#include <QObject>

inline bool operator<(const std::unique_ptr<MyPlotItem>& unique, const MyPlotItem* raw) {
	return unique.get() < raw;
}

inline bool operator<(const MyPlotItem* raw, const std::unique_ptr<MyPlotItem>& unique) {
	return raw < unique.get();
}
class PlotItemsContainer : public QObject {
	Q_OBJECT
	std::set<std::unique_ptr<MyPlotItem>, std::less<>> items_;
public:

	void remove(MyPlotItem* item) noexcept {
		auto it = items_.find(item->root());	
		if (it != items_.end()) {
			items_.erase(it);
			emit containerChanged();
		}
	}

	void clear() noexcept {
		items_.clear();
		emit containerChanged();
	}

	void add(std::unique_ptr<MyPlotItem> item) noexcept {
		items_.insert(std::move(item));
		emit containerChanged();
	}

	auto begin() noexcept {
		return items_.begin();
	}

	auto end() noexcept {
		return items_.end();
	}
signals:
	void containerChanged() const;
};