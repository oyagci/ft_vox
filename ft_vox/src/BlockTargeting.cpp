#include "BlockTargeting.hpp"
#include "Game.hpp"
#include <utility>
#include <limits>

void TargetBlockMesh::draw(glm::vec3 position)
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(1.0, 1.0, 1.0));

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	_Shader.bind();
	_Shader.setUniform4x4f("viewProjectionMatrix", Game::get().getWorld().getCamera().getViewProjectionMatrix());
	_Shader.setUniform4x4f("modelMatrix", model);
	glBindVertexArray(_Vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	_Shader.unbind();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

BlockTargeting::BlockTargeting()
{
}

void BlockTargeting::update(glm::vec3 const &position, glm::vec3 const &direction)
{
	auto chunks = Game::get().getWorld().getChunks().query(AxisAlignedBoundingBox2D{ { position.x, position.z }, 128.0f });

	Ray ray{ position, direction };
	HitResult shortestHit{ std::numeric_limits<float>::infinity(), 0.0f };

	for (auto chunk : chunks) {
		auto &octree = chunk->getOctree();
		HitResult hitResult = raycastAgainstOctree(*octree, ray);

		if (hitResult.Near < shortestHit.Near) {
			shortestHit = hitResult;
		}
	}

	if (shortestHit.Near < 24.0f) {
		glm::vec3 blockPosition(position + direction * (shortestHit.Near + 0.001f));
		_currentTarget.Position = floor(blockPosition);
		_currentTarget.Type = shortestHit.Id;
		_currentTarget.Normal = shortestHit.Normal;
	}
	else {
		_currentTarget.Position = { 0.0f, -1000.0f, 0.0f };
		_currentTarget.Type = 0;
	}
}

HitResult BlockTargeting::raycastAgainstOctree(octree::Octree const &octree, Ray const &ray)
{
	HitResult hitResult{ std::numeric_limits<float>::infinity(), 0.0f };

#if defined(DEBUG) && defined(DEBUG_OCTREE)
	octree::Octree const *last = nullptr;
#endif

	// Is the ray going through this chunk?
	hitResult = raycast(octree.boundary(), ray);

	if (hitResult.Near == std::numeric_limits<float>::infinity()) { // No hit
		return hitResult;
	}
	else if (!octree.is_divided()) { // Leaf
		if (octree.value() != 0) { // Hit
			hitResult.Id = octree.value();
			return hitResult;
		}

		// No hit;
		return { std::numeric_limits<float>::infinity(), 0.0f };
	}
	else { // Node

		std::vector<octree::Octree const*> hits;
		HitResult shortestHit{ std::numeric_limits<float>::infinity(), 0.0f };

		for (auto const &node : octree.nodes()) {
			HitResult result = raycast(node->boundary(), ray);
			hits.push_back(node);
		}

		for (auto const &h : hits) {
			auto result = raycastAgainstOctree(*h, ray);

			if (result.Near < shortestHit.Near) {
				shortestHit = result;
#if defined(DEBUG) && defined(DEBUG_OCTREE)
				last = h;
#endif
			}
		}

#if defined(DEBUG) && defined(DEBUG_OCTREE)
		if (last) last->draw();
#endif

		return shortestHit;
	}

	throw std::runtime_error("unreachable");
}

/// Do a raycast against an AABB
HitResult BlockTargeting::raycast(AxisAlignedBoundingBox const &aabb, Ray const &ray)
{
	glm::vec3 const &min = aabb.min();
	glm::vec3 const &max = aabb.max();

	HitResult hitResult{ std::numeric_limits<float>::infinity(), 0.0f };

	float t1 = (min.x - ray.Origin.x) / ray.Direction.x;
	float t2 = (max.x - ray.Origin.x) / ray.Direction.x;
	float t3 = (min.y - ray.Origin.y) / ray.Direction.y;
	float t4 = (max.y - ray.Origin.y) / ray.Direction.y;
	float t5 = (min.z - ray.Origin.z) / ray.Direction.z;
	float t6 = (max.z - ray.Origin.z) / ray.Direction.z;

	// Largest minimum
	float tmin = fmax(
		fmax(
			fmin(t1, t2),
			fmin(t3, t4)
		),
		fmin(t5, t6)
	);

	// Smallest maximum
	float tmax = fmin(
		fmin(
			fmax(t1, t2),
			fmax(t3, t4)
		),
		fmax(t5, t6)
	);

	if (tmax < 0.0f ||
		tmin > tmax) {
		return hitResult;
	}
	else if (tmin < 0.0f) {
		hitResult.Near = 0.0f;
		hitResult.Far = tmax;
		return hitResult;
	}
	else {
		hitResult.Near = tmin;
		hitResult.Far = tmax - tmin;
		hitResult.Normal = aabb.normalFromPoint(ray.Origin + ray.Direction * tmin);
	}

	return hitResult;
}

auto BlockTargeting::getCurrentTarget() -> std::optional<TargetBlockInfo>
{
	// Return a value without conditional branching

	std::optional<TargetBlockInfo> results[2] = { std::nullopt, std::make_optional(_currentTarget) };

	return results[static_cast<size_t>(_currentTarget.Position.y >= 0.0f)];
}
