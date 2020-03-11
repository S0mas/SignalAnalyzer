#pragma once
#include <map>
#include <set>
#include <numeric>
#include <algorithm>
#include <QDebug>
#include "MyPlotIntervalCurve.h"

using Position = int;

template<typename T>
class Positioner {
	std::map<Position, T*> items_;
	std::map<const T*, Position> items_2;

	bool contains(const T* obj) const noexcept {
		return items_2.find(obj) != items_2.end();
	}
public:
	Positioner() {}

	Position freePosition() const noexcept  {
		return items_.rbegin() == items_.rend() ? 0 : items_.rbegin()->first + 1;
	}

	Position add(T* item) noexcept {
		auto pos = freePosition();
		items_[pos] = item;
		items_2[items_[pos]] = pos;
		return pos;
	}

	void remove(const Position& posToRemove) noexcept {
		if (!isFree(posToRemove)) {
			items_2.erase(items_[posToRemove]);
			items_.erase(posToRemove);			
		}
	}

	void remove(const T* itemToRemove) noexcept {
		if (contains(itemToRemove)) {
			items_.erase(items_2[itemToRemove]);
			items_2.erase(itemToRemove);
		}
	}

	int size() const noexcept {
		return items_.size();
	}

	Position position(const T* obj) const noexcept {
		if (contains(obj))
			return items_2.at(obj);
		return 0xFFFFFFFF;
	}

	void show() const noexcept {
		for (auto item : items_)
			qDebug() << item.first << "  " << *item.second;
	}

	void move(const MyPlotItem* itemToMove, const QPointF& destination) noexcept {
		move({ itemToMove }, destination);
	}

	void move(std::vector<MyPlotItem*> items, const QPointF& destination) noexcept {
		std::set<Position> exclusiveToMove;
		std::set<MyPlotItem*> elseToMove;
		for (auto item : items)
			if(item->isPositionedExclusive())
				exclusiveToMove.insert(position(item));
			else
				elseToMove.insert(item);

		if (!exclusiveToMove.empty()) {
			if (floor(destination.y()) > *exclusiveToMove.crbegin())
				for (auto it = exclusiveToMove.rbegin(); it != exclusiveToMove.rend(); ++it)
					moveUp(*it);
			else if (floor(destination.y()) < *exclusiveToMove.cbegin())
				for (auto it = exclusiveToMove.begin(); it != exclusiveToMove.end(); ++it)
					moveDown(*it);
		}

		for (auto it = elseToMove.begin(); it != elseToMove.end(); ++it)
			(*it)->move(destination);
	}

	void moveUp(const Position& posToMove) noexcept {
		swap(posToMove, posToMove + 1);
	}

	void moveDown(const Position& posToMove) noexcept {
		swap(posToMove, posToMove - 1);
	}

	bool isFree(const Position& pos) const noexcept {
		return items_.find(pos) == items_.end();
	}

	T* curve(const Position& pos) const noexcept {
		return items_.find(pos) == items_.end() ? nullptr : items_.at(pos);
	}

	void swap(const Position& lhs, const Position& rhs) noexcept {
		T* left = isFree(lhs) ? nullptr : items_[lhs];
		T* right = isFree(rhs) ? nullptr : items_[rhs];
		if (left && right) {
			std::swap(items_2[items_[lhs]], items_2[items_[rhs]]);
			std::swap(items_[lhs], items_[rhs]);
		}
		else if (left) {
			items_2[items_[lhs]] = rhs;
			items_[rhs] = items_[lhs];
			items_.erase(lhs);
		}
		else if (right) {
			items_2[items_[rhs]] = lhs;
			items_[lhs] = items_[rhs];
			items_.erase(lhs);
		}
	}

	void select(const Position& posToSelect) noexcept {
			if (!isFree(posToSelect))
				items_[posToSelect]->select();
	}

	void select(const std::set<Position>& posToSelect) noexcept {
		for (auto it = posToSelect.begin(); it != posToSelect.end(); ++it)
			select(*it);
	}

	void deselect() noexcept {
		for (auto item : items_)
			item.second->deselect();
	}


	void deselect(const Position& posToDeselect) noexcept {
		if (!isFree(posToDeselect))
			item[posToDeselect]->deselect();
	}
};