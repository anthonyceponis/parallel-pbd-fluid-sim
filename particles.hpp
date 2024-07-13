#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Particles {
	const uint32_t particle_count;
	float radius = 0.01f;
	std::vector<glm::vec2> positions;
	std::vector<glm::vec2> prev_positions;
	std::vector<glm::vec2> velocities;
	std::vector<glm::vec2> gradients;
	std::vector<std::vector<uint32_t>> neighbours;
	std::vector<uint32_t> neighbour_counts;
	std::vector<float> densities;
	
	Particles(const uint32_t _particle_count);
};
