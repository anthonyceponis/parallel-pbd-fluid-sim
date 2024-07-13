#pragma once

#include <optional>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include "particles.hpp"

struct SpatialGridTwo {
	const uint32_t table_size = 370111;
	std::vector<std::optional<uint32_t>> first;
	std::vector<uint32_t> marks;
	uint32_t current_mark = 0;
	std::vector<std::optional<uint32_t>> next;
	Particles &particles;
	const float cell_size = 0.045f;

	std::vector<uint32_t> neighbours;
	std::vector<uint32_t> first_neighbour;

	SpatialGridTwo(Particles &_particles);

	uint32_t hashPos(const glm::vec2 pos);
	uint32_t hashCoords(const glm::ivec2 coords);
	glm::ivec2 posToCoords(const glm::vec2 pos);	

	void findNeighbours();
};
