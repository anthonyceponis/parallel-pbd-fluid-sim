#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Particles {
	uint32_t particle_count;
    const uint32_t max_particle_count;
	std::vector<glm::vec2> positions;
	std::vector<glm::vec2> prev_positions;
	std::vector<glm::vec2> velocities;
	std::vector<glm::vec2> gradients;
	std::vector<uint32_t> neighbours; // 2d flattened.
	std::vector<uint32_t> neighbour_counts;
	std::vector<float> densities;
	
	Particles(const uint32_t _particle_count, const uint32_t _max_particle_count);
};
