#pragma once

namespace std {
	template <>
	struct hash<glm::vec2>
	{
		std::size_t operator()(const glm::vec2 &v) const
		{
			return (hash<float>()(v.x)
					^ ((hash<float>()(v.y) << 1) >> 1));
		}
	};

	template <>
	struct hash<glm::ivec2>
	{
		std::size_t operator()(const glm::ivec2 &v) const
		{
			return (hash<float>()(v.x)
					^ ((hash<float>()(v.y) << 1) >> 1));
		}
	};

	template <>
	struct hash<glm::ivec3>
	{
		std::size_t operator()(const glm::ivec3 &v) const
		{
			return (hash<float>()(v.x)
					^ ((hash<float>()(v.y) << 1) >> 1)
					^ ((hash<float>()(v.z) << 2) >> 2));
		}
	};
}
