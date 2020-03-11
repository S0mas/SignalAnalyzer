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
	std::map<Position, T*> itemsExclusive_;
	std::map<const T*, Position> itemsExclusive_2;

	bool contains(const T* obj) const noexcept {
		return itemsExclusive_2.find(obj) != itemsExclusive_2.end();
	}
public:
	Positioner() {}

	Position freePosition() const noexcept  {
		return itemsExclusive_.rbegin() == itemsExclusive_.rend() ? 0 : itemsExclusive_.rbegin()->first + 1;
	}

	Position addExclusive(T* item) noexcept {
		auto pos = freePosition();
		itemsExclusive_[pos] = item;
		itemsExclusive_2[itemsExclusive_[pos]] = pos;
		return pos;
	}

	void remove(const Position& posToRemove) noexcept {
		if (!isFree(posToRemove)) {
			itemsExclusive_2.erase(itemsExclusive_[posToRemove]);
			itemsExclusive_.erase(posToRemove);			
		}
	}

	void remove(const T* itemToRemove) noexcept {
		if (contains(itemToRemove)) {
			itemsExclusive_.erase(itemsExclusive_2[itemToRemove]);
			itemsExclusive_2.erase(itemToRemove);
		}
	}

	int size() const noexcept {
		return itemsExclusive_.size();
	}

	Position position(const T* obj) const noexcept {
		if (contains(obj))
			return itemsExclusive_2.at(obj);
		return 0xFFFFFFFF;
	}

	void show() const noexcept {
		for (auto item : itemsExclusive_)
			qDebug() << item.first << "  " << *item.second;
	}

	void move(const MyPlotItem* itemToMove, const QPointF& destination) noexcept {
		move({ itemToMove }, destination);
	}

	void move(std::vector<MyPlotItem*> items, const QPointF& destination) noexcept {
		std::set<Position> exclusiveToMove;
		for (auto item : items)
			if(item->isPositionedExclusive())
				exclusiveToMove.insert(position(item));

		if (!exclusiveToMove.empty()) {
			if (floor(destination.y()) > *exclusiveToMove.crbegin())
				for (auto it = exclusiveToMove.rbegin(); it != exclusiveToMove.rend(); ++it)
					moveUp(*it);
			else if (floor(destination.y()) < *exclusiveToMove.cbegin())
				for (auto it = exclusiveToMove.begin(); it != exclusiveToMove.end(); ++it)
					moveDown(*it);
		}
	}

	void moveUp(const Position& posToMove) noexcept {
		swap(posToMove, posToMove + 1);
	}

	void moveDown(const Position& posToMove) noexcept {
		swap(posToMove, posToMove - 1);
	}

	bool isFree(const Position& pos) const noexcept {
		return itemsExclusive_.find(pos) == itemsExclusive_.end();
	}

	T* curve(const Position& pos) const noexcept {
		return itemsExclusive_.find(pos) == itemsExclusive_.end() ? nullptr : itemsExclusive_.at(pos);
	}

	void swap(const Position& lhs, const Position& rhs) noexcept {
		T* left = isFree(lhs) ? nullptr : itemsExclusive_[lhs];
		T* right = isFree(rhs) ? nullptr : itemsExclusive_[rhs];
		if (left && right) {
			std::swap(itemsExclusive_2[itemsExclusive_[lhs]], itemsExclusive_2[itemsExclusive_[rhs]]);
			std::swap(itemsExclusive_[lhs], itemsExclusive_[rhs]);
		}
		else if (left) {
			itemsExclusive_2[itemsExclusive_[lhs]] = rhs;
			itemsExclusive_[rhs] = itemsExclusive_[lhs];
			itemsExclusive_.erase(lhs);
		}
		else if (right) {
			itemsExclusive_2[itemsExclusive_[rhs]] = lhs;
			itemsExclusive_[lhs] = itemsExclusive_[rhs];
			itemsExclusive_.erase(lhs);
		}
	}

	void select(const Position& posToSelect) noexcept {
			if (!isFree(posToSelect))
				itemsExclusive_[posToSelect]->select();
	}

	void select(const std::set<Position>& posToSelect) noexcept {
		for (auto it = posToSelect.begin(); it != posToSelect.end(); ++it)
			select(*it);
	}

	void deselect() noexcept {
		for (auto item : itemsExclusive_)
			item.second->deselect();
	}


	void deselect(const Position& posToDeselect) noexcept {
		if (!isFree(posToDeselect))
			item[posToDeselect]->deselect();
	}
};