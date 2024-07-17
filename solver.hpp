#pragma once

#include "particles.hpp"
#include "spatial_grid.hpp"
#include "constants.hpp"

struct Solver {
	Particles &particles;
	glm::vec2 world_size;
	SpatialGrid *spatial_grid;

	Solver(glm::vec2 _world_size, Particles &_particles);

	~Solver();

	void update();
	
	void findNeighbours();
	
	void solveFluid();

	void solveBoundaries();

	float poly6Kernel2D(const uint32_t particle_id, const uint32_t neighbour_particle_id);

	float spikyKernelGrad2D(const uint32_t particle_id, const uint32_t neighbour_particle_id);
	
	
};
