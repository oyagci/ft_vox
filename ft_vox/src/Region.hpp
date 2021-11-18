#pragma once

#include "AxisAlignedBoundingBox.hpp"
#include <array>
#include <glm/vec2.hpp>
#include <optional>
#include <ostream>
#include <vector>
#include <algorithm>

template <typename ElemType>
class RegionNode
{
public:
	struct Elem {
		glm::vec2 Position;
		ElemType Data;
	};
public:
	RegionNode(AxisAlignedBoundingBox2D const &boundary)
		: _Boundary(boundary) {}

	bool push(glm::vec2 position, ElemType elem)
	{
		if (_Boundary.contains(position)) {
			_Elements.push_back({ position, elem });
			return true;
		}
		return false;
	}

	void erase(glm::vec2 position)
	{
		if (!_Boundary.contains(position)) { return; }

		std::vector<Elem> elements;

		for (size_t i = 0; i < _Elements.size(); i++) {
			if (_Elements[i].Position != position) {
				elements.push_back(_Elements[i]);
			}
		}

		_Elements = elements;
	}

	auto const &boundary() const
	{
		return _Boundary;
	}

	auto const &elements() const
	{
		return _Elements;
	}

	auto &elements()
	{
		return _Elements;
	}

	auto empty() const
	{
		return _Elements.size() == 0;
	}

private:
	AxisAlignedBoundingBox2D _Boundary;
	std::vector<Elem> _Elements;
};

template <typename ElemType>
class Region
{
private:
	using RegionNodeType = RegionNode<ElemType>;

	static constexpr size_t RegionSideSize = 3 * 64;
	static constexpr float  RegionSize     = RegionSideSize / 2.0f;

public:

	float roundUp(float number, float fixedBase) {
		if (number > 0.0f) {
			return (int)(number / fixedBase) * fixedBase;
		}

		if (fixedBase != 0 && number != 0) {
			float sign = number > 0.0f ? 1.0f : -1.0f;
			number *= sign;
			number /= fixedBase;
			int fixedPoint = (int)ceil(number);
			number = fixedPoint * fixedBase;
			number *= sign;
		}
		return number;
	}

	void set(glm::vec2 position, ElemType elem)
	{
		for (auto &r : _RegionNodes) {
			if (r.push(position, elem)) {
				return;
			}
		}

		// No region containing `position' found. We must create it.

		// Align the position on RegionSideSize
		glm::ivec2 regionChunk{
			(int)roundUp(floor(position.x), (float)RegionSideSize),
			(int)roundUp(floor(position.y), (float)RegionSideSize),
		};

		glm::ivec2 regionCenter{ regionChunk.x + RegionSize, regionChunk.y + RegionSize };

		auto &region = _RegionNodes.emplace_back(
			RegionNodeType(
				AxisAlignedBoundingBox2D{ regionCenter, RegionSize }
			));

		assert(region.push(position, elem));
	}

	std::vector<ElemType> query(glm::vec2 const &position) const
	{
		std::vector<typename RegionNodeType::Elem> elements;

		for (auto const &r : _RegionNodes) {
			if (r.boundary().contains(position)) {

				auto match = std::find_if(r.elements().begin(), r.elements().end(), [&position](auto const &elem) -> bool {
					return ((position.x >= elem.Position.x && position.x < elem.Position.x + 64) &&
							(position.y >= elem.Position.y && position.y < elem.Position.y + 64));
				});

				if (match != r.elements().end()) {
					elements.push_back(*match);
				}
			}
		}

		std::vector<ElemType> elemData;
		std::transform(elements.begin(), elements.end(), std::back_inserter(elemData), [](auto &e) { return e.Data; });

		return elemData;
	}

	std::vector<ElemType> query(AxisAlignedBoundingBox2D const &boundary) const
	{
		std::vector<typename RegionNodeType::Elem> elements;

		for (auto const &r : _RegionNodes) {
			if (boundary.contains(r.boundary())) {
				elements.insert(elements.end(), r.elements().begin(), r.elements().end());
			}
		}

		std::vector<ElemType> elemData;
		std::transform(elements.begin(), elements.end(), std::back_inserter(elemData), [](auto &e) { return e.Data; });

		return elemData;
	}

	std::vector<ElemType> queryOutside(AxisAlignedBoundingBox2D const &boundary) const
	{
		std::vector<typename RegionNodeType::Elem> elements;

		for (auto const &r : _RegionNodes) {
			if (!r.boundary().contains(boundary)) {
				elements.insert(elements.end(), r.elements().begin(), r.elements().end());
			}
		}

		std::vector<ElemType> elemData;
		std::transform(elements.begin(), elements.end(), std::back_inserter(elemData), [](auto &e) { return e.Data; });

		return elemData;
	}

	void erase(glm::vec2 const &position)
	{
		std::vector<RegionNodeType> regions;

		for (auto &r : _RegionNodes) {
			r.erase(position);
			if (!r.empty()) {
				regions.push_back(r);
			}
		}

		_RegionNodes = regions;
	}

private:
	std::vector<RegionNodeType> _RegionNodes;
};
