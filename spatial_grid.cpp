#include "spatial_grid.hpp"
#include "particles.hpp"

#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

SpatialGrid::SpatialGrid(Particles &_particles) : 
particles(_particles), 
cell_size(0.045f),
lookup(_particles.max_particle_count){
	this->table_size = 2 * this->particles.max_particle_count;
	this->counts.resize(this->table_size + 1, 0);
};

void SpatialGrid::build() {	
	// Reset.
	for (uint32_t i = 0; i < this->counts.size(); i++) this->counts[i] = 0;

	// Compute counts.
	for (uint32_t i = 0; i < this->particles.particle_count; i++) {
		const uint32_t h = this->hashPos(this->particles.positions[i]);
		
		this->counts[h]++;	
	}

	// Compute partial sums.
	for (uint32_t i = 1; i < this->counts.size(); i++) {
		this->counts[i] += this->counts[i-1];
	}

	// Fill lookup table.
	for (uint32_t i = 0; i < this->particles.particle_count; i++) {
		const uint32_t h = this->hashPos(this->particles.positions[i]);
		
		this->counts[h]--;
		this->lookup[this->counts[h]] = i;
	}	
};

uint32_t SpatialGrid::query(const uint32_t particle_id) {
	const glm::vec2 coords = this->posToCoords(this->particles.positions[particle_id]);

	uint32_t query_size = 0;
	
	for (int32_t xi = coords.x-1; xi <= coords.x+1; xi++) {
		for (int32_t yi = coords.y-1; yi <= coords.y+1; yi++) {
			const uint32_t h = this->hashCoords(glm::ivec2(xi, yi));			
			const uint32_t start = this->counts[h];
			const uint32_t end = this->counts[h+1];		

			for (uint32_t i = start; i < end; i++) {
				this->particles.neighbours[particle_id * this->particles.particle_count + query_size] = this->lookup[i];
				query_size++;
			}

		}
	}

	return query_size;
};

uint32_t SpatialGrid::hashCoords(const glm::ivec2 coords) {
	// Must convert to unsigned int so that overflows wrap around.
	const int32_t h = static_cast<uint32_t>(coords.x * 92837111)  ^ static_cast<uint32_t>(coords.y * 689287499) ; // Aribtrary hash function.
	return abs(h) % this->table_size;
};

glm::ivec2 SpatialGrid::posToCoords(const glm::vec2 pos) {
	const int32_t cell_x = static_cast<int32_t>((pos.x + 100.0f) / this->cell_size);
	const int32_t cell_y = static_cast<int32_t>((pos.y + 1.0f) / this->cell_size);
	return glm::ivec2(cell_x, cell_y);
}

uint32_t SpatialGrid::hashPos(const glm::vec2 pos) {
	return this->hashCoords(this->posToCoords(pos));
}

