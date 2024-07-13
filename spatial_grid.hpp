#pragma once

#include "particles.hpp"
#include <glm/glm.hpp>
#include <vector>

struct SpatialGrid {
	Particles &particles;
	float cell_size;
	uint32_t table_size;
	std::vector<uint32_t> counts;
	std::vector<uint32_t> lookup;
	
 	SpatialGrid(Particles &_particles);

	void build();	
	uint32_t query(const uint32_t particle_id); // Returns size of query.
	uint32_t hashCoords(const glm::ivec2 coords); // For hashing coordinates of spatial grid.
	uint32_t hashPos(const glm::vec2 pos); // For hashing a position.
	glm::ivec2 posToCoords(const glm::vec2 pos);

};
