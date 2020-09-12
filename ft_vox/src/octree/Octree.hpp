#pragma once

#include <array>
#include <memory>
#include <glm/vec3.hpp>
#include <optional>
#include <iostream>
#include <algorithm>
#include "AxisAlignedBoundingBox.hpp"
#include "DebugGrid.hpp"

namespace octree {

	class Octree
	{
	public:
		virtual ~Octree() {}

		virtual void set(int x, int y, int z, int val) = 0;
		virtual void set_local(int x, int y, int z, int val) = 0;
		virtual void erase(int x, int y, int z) = 0;
		virtual std::optional<int> at(int x, int y, int z) = 0;
		virtual std::optional<int> at_local(int x, int y, int z) = 0;
		virtual void collapse() = 0;
		virtual int value() const = 0;

		virtual AxisAlignedBoundingBox const &boundary() const = 0;
		virtual AxisAlignedBoundingBox &boundary() = 0;

		virtual std::array<Octree*, 8> nodes() = 0;
		virtual std::array<Octree const*, 8> const nodes() const = 0;

		virtual bool is_divided() const = 0;

		virtual void draw() const = 0;
	};

	class OctreeNode : public Octree
	{
	private:
		static constexpr size_t LEFT_BOTTOM_FRONT  = 0;
		static constexpr size_t RIGHT_BOTTOM_FRONT = 1;
		static constexpr size_t LEFT_TOP_FRONT     = 2;
		static constexpr size_t RIGHT_TOP_FRONT    = 3;
		static constexpr size_t LEFT_BOTTOM_BACK   = 4;
		static constexpr size_t RIGHT_BOTTOM_BACK  = 5;
		static constexpr size_t LEFT_TOP_BACK      = 6;
		static constexpr size_t RIGHT_TOP_BACK     = 7;

	private:
		void divide()
		{
			float dividedSize = _Boundary.Size / 2.0f;

			_Divided = true;

			_Nodes[LEFT_BOTTOM_FRONT] = std::make_unique<OctreeNode>(
				glm::vec3{
					_Boundary.Position.x - dividedSize,
					_Boundary.Position.y - dividedSize,
					_Boundary.Position.z - dividedSize,
				}, dividedSize, _Value);
			_Nodes[RIGHT_BOTTOM_FRONT] = std::make_unique<OctreeNode>(
				glm::vec3{
					_Boundary.Position.x + dividedSize,
					_Boundary.Position.y - dividedSize,
					_Boundary.Position.z - dividedSize,
				}, dividedSize, _Value);
			_Nodes[LEFT_TOP_FRONT] = std::make_unique<OctreeNode>(
				glm::vec3{
					_Boundary.Position.x - dividedSize,
					_Boundary.Position.y + dividedSize,
					_Boundary.Position.z - dividedSize,
				}, dividedSize, _Value);
			_Nodes[RIGHT_TOP_FRONT] = std::make_unique<OctreeNode>(
				glm::vec3{
					_Boundary.Position.x + dividedSize,
					_Boundary.Position.y + dividedSize,
					_Boundary.Position.z - dividedSize,
				}, dividedSize, _Value);
			_Nodes[LEFT_BOTTOM_BACK] = std::make_unique<OctreeNode>(
				glm::vec3{
					_Boundary.Position.x - dividedSize,
					_Boundary.Position.y - dividedSize,
					_Boundary.Position.z + dividedSize,
				}, dividedSize, _Value);
			_Nodes[RIGHT_BOTTOM_BACK] = std::make_unique<OctreeNode>(
				glm::vec3{
					_Boundary.Position.x + dividedSize,
					_Boundary.Position.y - dividedSize,
					_Boundary.Position.z + dividedSize,
				}, dividedSize, _Value);
			_Nodes[LEFT_TOP_BACK] = std::make_unique<OctreeNode>(
				glm::vec3{
					_Boundary.Position.x - dividedSize,
					_Boundary.Position.y + dividedSize,
					_Boundary.Position.z + dividedSize,
				}, dividedSize, _Value);
			_Nodes[RIGHT_TOP_BACK] = std::make_unique<OctreeNode>(
				glm::vec3{
					_Boundary.Position.x + dividedSize,
					_Boundary.Position.y + dividedSize,
					_Boundary.Position.z + dividedSize,
				}, dividedSize, _Value);
		}

		void collapse()
		{
			if (!_Divided) { return; }

			bool noneDivided = std::none_of(_Nodes.begin(), _Nodes.end(), [](auto &n) -> bool {
				return n->is_divided();
			});
			bool allEqual = std::all_of(_Nodes.begin(), _Nodes.end(), [&](auto &n) -> bool {
				return _Nodes[0]->_Value == n->_Value;
			});

			if (noneDivided && allEqual) {
				_Value = _Nodes[0]->_Value;
				_Divided = false;
				for (auto &node : _Nodes) {
					node.reset(nullptr);
				}
			}
		}

	public:
		OctreeNode() = delete;

		OctreeNode(glm::vec3 position, float size, int defaultValue = 0)
			: _Boundary{position, size}, _Nodes{}, _Value(defaultValue), _Divided(false)
		{
		}

		void set(int x, int y, int z, int val) override
		{
			if (!_Boundary.contains({ x, y, z })) { return; }
			if (_Value == val) { return; }

			if (_Boundary.Size >= 1 && !_Divided) {
				divide();
			}

			if (_Divided) {
				for (auto const &node : _Nodes) {
					node->set(x, y, z, val);
				}
				collapse();
			}
			else {
				_Value = val;
			}
		}

		void set_local(int x, int y, int z, int val) override
		{
			// Convert local coordinates to world coordinates
			x += _Boundary.Position.x - _Boundary.Size;
			y += _Boundary.Position.y - _Boundary.Size;
			z += _Boundary.Position.z - _Boundary.Size;

			set(x, y, z, val);
		}

		std::optional<int> at(int x, int y, int z) override
		{
			if (_Boundary.contains({ x, y, z })) {
				if (_Divided) {
					for (auto const &node : _Nodes) {
						auto res = node->at(x, y, z);
						if (res) {
							return res;
						}
					}
					return std::nullopt;
				}
				return std::make_optional(_Value);
			}

			return std::nullopt;
		}

		std::optional<int> at_local(int x, int y, int z) override
		{
			// Convert local coordinates to world coordinates
			x += _Boundary.Position.x - _Boundary.Size;
			y += _Boundary.Position.y - _Boundary.Size;
			z += _Boundary.Position.z - _Boundary.Size;

			return at(x, y, z);
		}

		void erase(int x, int y, int z) override {}

		AxisAlignedBoundingBox const &boundary() const override { return _Boundary; }
		AxisAlignedBoundingBox &boundary() override { return _Boundary; }

		std::array<Octree*, 8> nodes() override
		{
			std::array<Octree*, 8> arr{ nullptr };

			std::transform(_Nodes.begin(), _Nodes.end(), arr.begin(), [] (std::unique_ptr<OctreeNode> &u_ptr) {
				return u_ptr.get();
			});

			return arr;
		}

		std::array<Octree const*, 8> const nodes() const override
		{
			std::array<Octree const*, 8> arr{ nullptr };

			std::transform(_Nodes.begin(), _Nodes.end(), arr.begin(), [] (std::unique_ptr<OctreeNode> const &u_ptr) {
				return u_ptr.get();
			});

			return arr;
		}

		bool is_divided() const override
		{
			return _Divided;
		}

		int value() const override
		{
			return _Value;
		}

		void draw() const override
		{
			if (_Boundary.Size <= 0.5f || _Divided)
				DebugGrid::get().draw(_Boundary.Position - glm::vec3(_Boundary.Size, _Boundary.Size, _Boundary.Size), _Boundary.Size * 2, 0.1f);
		}

	private:
		AxisAlignedBoundingBox _Boundary;
		std::array<std::unique_ptr<OctreeNode>, 8> _Nodes;
		int _Value;
		bool _Divided;

	};
}
