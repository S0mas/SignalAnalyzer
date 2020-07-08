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
	std::map<Position, std::vector<T*>> positionToItemsMap_;
	std::map<const T*, Position> itemToPositionMap_;

	bool contains(const T* obj) const noexcept {
		return itemToPositionMap_.find(obj) != itemToPositionMap_.end();
	}
public:
	Positioner() {}

	Position freePosition() const noexcept  {
		return positionToItemsMap_.rbegin() == positionToItemsMap_.rend() ? 0 : positionToItemsMap_.rbegin()->first + 1;
	}

	Position addExclusive(T* item) noexcept {
		auto pos = freePosition();
		positionToItemsMap_[pos].push_back(item);
		itemToPositionMap_[item] = pos;
		return pos;
	}

	void clear() noexcept {
		itemToPositionMap_.clear();
		positionToItemsMap_.clear();
	}

	void restoreExclusivePosition(const T* item) noexcept {
		auto& items = positionToItemsMap_[position(item)];
		while (items.size() > 1) {
			auto toReposition = items.back();
			items.pop_back();
			auto newPosition = freePosition();
			positionToItemsMap_[newPosition].push_back(toReposition);
			itemToPositionMap_[toReposition] = newPosition;
		}
	}

	void overlap(const T* base, T* toOverlap) noexcept {
		auto oldPosition = itemToPositionMap_[toOverlap];
		auto overlapedPosition = itemToPositionMap_[base];
		if(oldPosition == overlapedPosition)
			return;
		for (auto item : positionToItemsMap_[oldPosition]) {
			itemToPositionMap_[item] = overlapedPosition;
			positionToItemsMap_[overlapedPosition].push_back(item);
		}
		positionToItemsMap_.erase(oldPosition);
	}

	void remove(const Position& posToRemove) noexcept {
		if (!isFree(posToRemove)) {
			itemToPositionMap_.erase(positionToItemsMap_[posToRemove]);
			positionToItemsMap_.erase(posToRemove);			
		}
	}

	void remove(const T* itemToRemove) noexcept {
		if (contains(itemToRemove)) {
			positionToItemsMap_.erase(itemToPositionMap_[itemToRemove]);
			itemToPositionMap_.erase(itemToRemove);
		}
	}

	int size() const noexcept {
		return positionToItemsMap_.size();
	}

	Position position(const T* obj) const noexcept {
		if (contains(obj))
			return itemToPositionMap_.at(obj);
		return 0xFFFFFFFF;
	}

	void show() const noexcept {
		for (auto item : positionToItemsMap_)
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
		if (auto movedGroup = curves(posToMove); movedGroup)
			for (auto movedCurve : *movedGroup)
				movedCurve->move(0, 1);
		if (auto movedGroup = curves(posToMove + 1); movedGroup)
			for (auto movedCurve : *movedGroup)
				movedCurve->move(0, -1);
	}

	void moveDown(const Position& posToMove) noexcept {
		swap(posToMove, posToMove - 1);
		if (auto movedGroup = curves(posToMove); movedGroup)
			for (auto movedCurve : *movedGroup)
				movedCurve->move(0, - 1);
		if (auto movedGroup = curves(posToMove - 1); movedGroup)
			for (auto movedCurve : *movedGroup)
				movedCurve->move(0, 1);
	}

	bool isFree(const Position& pos) const noexcept {
		return positionToItemsMap_.find(pos) == positionToItemsMap_.end();
	}

	T* curve(const Position& pos) const noexcept {
		return positionToItemsMap_.find(pos) == positionToItemsMap_.end() ? nullptr : positionToItemsMap_.at(pos).back();
	}

	const std::vector<T*>* curves(const Position& pos) const noexcept {
		return positionToItemsMap_.find(pos) == positionToItemsMap_.end() ? nullptr : &positionToItemsMap_.at(pos);
	}

	void swap(const Position& lhs, const Position& rhs) noexcept {
		std::vector<T*>* left = isFree(lhs) ? nullptr : &positionToItemsMap_[lhs];
		std::vector<T*>* right = isFree(rhs) ? nullptr : &positionToItemsMap_[rhs];
		if (left && right) {
			for (auto& item : positionToItemsMap_[rhs])
				itemToPositionMap_[item] = lhs;
			for (auto& item : positionToItemsMap_[lhs])
				itemToPositionMap_[item] = rhs;
			std::swap(positionToItemsMap_[lhs], positionToItemsMap_[rhs]);
		}
		else if (left) {
			for(auto& item : positionToItemsMap_[lhs])
				itemToPositionMap_[item] = rhs;	
			positionToItemsMap_[rhs] = positionToItemsMap_[lhs];
			positionToItemsMap_.erase(lhs);
		}
		else if (right) {
			for (auto& item : positionToItemsMap_[rhs])
				itemToPositionMap_[item] = lhs;
			positionToItemsMap_[lhs] = positionToItemsMap_[rhs];
			positionToItemsMap_.erase(lhs);
		}
	}

	void select(const Position& posToSelect) noexcept {
		if (!isFree(posToSelect)) {
			for (auto& item : positionToItemsMap_[posToSelect])
				item->setProperty("selected", true);
		}
	}

	void select(const std::set<Position>& posToSelect) noexcept {
		for (auto it = posToSelect.begin(); it != posToSelect.end(); ++it)
			select(*it);
	}

	void deselect() noexcept {
		for (auto& itemGroup : positionToItemsMap_) {
			for (auto& item : itemGroup.second)
				item->setProperty("selected", false);
		}
	}


	void deselect(const Position& posToDeselect) noexcept {
		if (!isFree(posToDeselect)) {
			for (auto& item : positionToItemsMap_[posToDeselect])
				item->setProperty("selected", false);
		}
	}
};