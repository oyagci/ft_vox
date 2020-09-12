#pragma once

#include <glm/vec3.hpp>
#include <utility>

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
};
