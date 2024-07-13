#include "spatial_grid_two.hpp"
#include <optional>

SpatialGridTwo::SpatialGridTwo(Particles &_particles) : 
particles(_particles),
first(this->table_size, std::nullopt),
marks(this->table_size, 0),
next(_particles.particle_count, std::optional<uint32_t>(0)),
neighbours(_particles.particle_count * 10),
first_neighbour(_particles.particle_count+1){}; 


void SpatialGridTwo::findNeighbours() {
	this->current_mark += 1;

	for (uint32_t i = 0; i < this->particles.particle_count; i++) {
		const uint32_t h = SpatialGridTwo::hashPos(this->particles.positions[i]);
		
		if (this->marks[h] != this->current_mark) {
			this->marks[h] = this->current_mark;
			this->first[h] = std::nullopt;
		}	

		this->next[i] = this->first[h];
		this->first[h] = i;
	}


	this->neighbours.clear();	
	for (uint32_t i = 0; i < this->particles.particle_count; i++) {
		glm::vec2 p = this->particles.positions[i];
		this->first_neighbour[i] = this->neighbours.size();
		const glm::ivec2 coords = SpatialGridTwo::posToCoords(p);
		for (int32_t x = coords.x-1; x <= coords.x+1; x++) {
			for (int32_t y = coords.y-1; y <= coords.y+1; y++) {
				const uint32_t h = SpatialGridTwo::hashCoords(glm::ivec2(x,y));
				if (this->marks[h] != this->current_mark) continue;
				std::optional<uint32_t> j = this->first[h];
				while (j.has_value()) {
					neighbours.push_back(j.value());
					j = this->next[j.value()];
				}
			}
		}
		
	}

	this->first_neighbour[this->particles.particle_count] = this->neighbours.size();
}

uint32_t SpatialGridTwo::hashCoords(const glm::ivec2 coords) {
	// Must convert to unsigned int so that overflows wrap around.
	const int32_t h = static_cast<uint32_t>(coords.x * 92837111)  ^ static_cast<uint32_t>(coords.y * 689287499) ; // Aribtrary hash function.
	return abs(h) % this->table_size;
};

glm::ivec2 SpatialGridTwo::posToCoords(const glm::vec2 pos) {
	const int32_t cell_x = static_cast<int32_t>(pos.x / this->cell_size);
	const int32_t cell_y = static_cast<int32_t>(pos.y / this->cell_size);
	return glm::ivec2(cell_x, cell_y);
}

uint32_t SpatialGridTwo::hashPos(const glm::vec2 pos) {
	return SpatialGridTwo::hashCoords(SpatialGridTwo::posToCoords(pos));
}

