#include "particles.hpp"

Particles::Particles(const uint32_t _particle_count, const uint32_t _max_particle_count) : 
particle_count(_particle_count),
max_particle_count(_max_particle_count),
positions(_max_particle_count),
prev_positions(_max_particle_count),
velocities(_max_particle_count),
gradients(_max_particle_count),
neighbours(_max_particle_count * _max_particle_count),
neighbour_counts(_max_particle_count),
densities(_max_particle_count)
{};

