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
	auto chunkOpt = Game::get().getWorld().getChunk({ 0 / Chunk::CHUNK_SIZE, 0 / Chunk::CHUNK_SIZE });
	if (!chunkOpt) { return; }

	auto &octree = *(**chunkOpt).getOctree();

	Ray ray{ position, direction };
	HitResult hitResult = raycastAgainstOctree(octree, ray);

	glm::vec3 blockPosition(position + direction * (hitResult.Near + 0.001f));
	_targetBlockPosition = floor(blockPosition);
}

HitResult BlockTargeting::raycastAgainstOctree(octree::Octree const &octree, Ray const &ray)
{
	HitResult hitResult{ std::numeric_limits<float>::infinity(), 0.0f };

#if defined(DEBUG) && defined(DEBUG_OCTREE)
	octree::Octree const *last = nullptr;
#endif

	if (octree.is_divided()) {

		std::vector<octree::Octree const*> hits;

		for (auto const &node : octree.nodes()) {

			AxisAlignedBoundingBox box{ node->boundary().Position, node->boundary().Size };
			HitResult result = raycast(box, ray);

			hits.push_back(node);
		}

		HitResult shortestHit{ std::numeric_limits<float>::infinity(), 0.0f };

		for (auto const &h : hits) {
			auto result = raycastAgainstOctree(*h, ray);

			if (result.Near < shortestHit.Near) {
				shortestHit = result;
#if defined(DEBUG) && defined(DEBUG_OCTREE)
				last = h;
#endif
			}
		}

		hitResult = shortestHit;
#if defined(DEBUG) && defined(DEBUG_OCTREE)
		if (last) last->draw();
#endif
	}
	else if (octree.value() != 0) {

		AxisAlignedBoundingBox box{ octree.boundary().Position, octree.boundary().Size };
		hitResult = raycast(box, ray);
	}

	return hitResult;
}

/// Do a raycast against an AABB
/// Return value: vec2 where `x' is the near distance and `y' is the far distance (the length of the ray inside the box).
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
	}

	return hitResult;
}