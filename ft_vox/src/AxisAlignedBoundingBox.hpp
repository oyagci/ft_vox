#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <utility>
#include <limits>
#include <cmath>

struct AxisAlignedBoundingBox
{
	glm::vec3 Position;
	float Size;

	static AxisAlignedBoundingBox fromMinMax(glm::vec3 const &min, glm::vec3 const &max)
	{
		return AxisAlignedBoundingBox{ (min + max) * 0.5f, ((max - min) * 0.5f).x };
	}

	bool contains(glm::vec3 point) const
	{
		return (
			point.x >= Position.x - Size &&
			point.y >= Position.y - Size &&
			point.z >= Position.z - Size &&
			point.x < Position.x + Size &&
			point.y < Position.y + Size &&
			point.z < Position.z + Size
		);
	}

	glm::vec3 min() const
	{
		glm::vec3 size(Size, Size, Size);

		glm::vec3 const p1 = glm::vec3(Position) + size;
		glm::vec3 const p2 = glm::vec3(Position) - size;

		return glm::vec3(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
	}

	glm::vec3 max() const
	{
		glm::vec3 size(Size, Size, Size);

		glm::vec3 const p1 = glm::vec3(Position) + size;
		glm::vec3 const p2 = glm::vec3(Position) - size;

		return glm::vec3(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
	}

	template <typename T> inline T sgn(T const val) const {
		return (T)(T(0) < (T)val) - ((T)val < T(0));
	}

	glm::vec3 normalFromPoint(glm::vec3 point) const
	{
		glm::vec3 normal{ 0.0f, 0.0f, 0.0f };
		float epsilon = std::numeric_limits<float>::max();
		float distance = 0.0f;

		point -= Position;

		distance = abs(Size - abs(point.x));
		if (distance < epsilon) {
			epsilon = distance;
			normal = sgn(point.x) * glm::vec3(1.0f, 0.0f, 0.0f);
		}

		distance = abs(Size - abs(point.y));
		if (distance < epsilon)
		{
			epsilon = distance;
			normal = sgn(point.y) * glm::vec3(0.0f, 1.0f, 0.0f);
		}

		distance = abs(Size - abs(point.z));
		if (distance < epsilon)
		{
			epsilon = distance;
			normal = sgn(point.z) * glm::vec3(0.0f, 0.0f, 1.0f);
		}

		return normal;
	}
};

struct AxisAlignedBoundingBox2D
{
	glm::vec2 Position;
	float Size;

	static AxisAlignedBoundingBox2D fromMinMax(glm::vec2 const &min, glm::vec2 const &max)
	{
		return AxisAlignedBoundingBox2D{ (min + max) * 0.5f, ((max - min) * 0.5f).x };
	}

	bool contains(glm::vec2 const &point) const
	{
		return (
			point.x >= Position.x - Size &&
			point.y >= Position.y - Size &&
			point.x < Position.x + Size &&
			point.y < Position.y + Size
		);
	}

	bool contains(AxisAlignedBoundingBox2D const &aBox) const
	{
		auto const aMin = aBox.min();
		auto const aMax = aBox.max();

		auto const bMin = min();
		auto const bMax = max();

		return ((aMin.x <= bMax.x && aMax.x >= bMin.x) &&
				(aMin.y <= bMax.y && aMax.y >= bMin.y));
	}

	glm::vec2 min() const
	{
		glm::vec2 size(Size, Size);

		glm::vec2 const p1 = Position + size;
		glm::vec2 const p2 = Position - size;

		return glm::vec2(fmin(p1.x, p2.x), fmin(p1.y, p2.y));
	}

	glm::vec2 max() const
	{
		glm::vec2 size(Size, Size);

		glm::vec2 const p1 = Position + size;
		glm::vec2 const p2 = Position - size;

		return glm::vec2(fmax(p1.x, p2.x), fmax(p1.y, p2.y));
	}
};
