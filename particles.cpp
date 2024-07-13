#include "particles.hpp"

Particles::Particles(const uint32_t _particle_count) : 
particle_count(_particle_count),
positions(_particle_count, glm::vec2(0.0f)),
prev_positions(_particle_count, glm::vec2(0.0f)),
velocities(_particle_count, glm::vec2(0.0f)),
gradients(_particle_count, glm::vec2(0.0f)),
neighbours(_particle_count, std::vector<uint32_t>(_particle_count, 0)),
neighbour_counts(_particle_count, 0),
densities(_particle_count, 0.0f){};
