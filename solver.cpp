#include "solver.hpp"
#include "spatial_grid.hpp"

#include <bits/stdc++.h>
#include "particles.hpp"

Solver::Solver(glm::vec2 _world_size, Particles &_particles) : 
particles(_particles),
neighbours(_particles.particle_count),
world_size(_world_size) {
	this->spatial_grid = new SpatialGrid(this->particles);
	
	// Setup particles.
	const float spacing = this->particles.radius * 1.0f;
	const glm::vec2 origin(0.1f, 3.0f);
	glm::vec2 pos = origin; 
	const uint32_t cols = 50;
	const uint32_t rows = this->particles.particle_count / cols;		
	
	for (uint32_t y = 0; y < rows; y++) {
		for (uint32_t x = 0; x < cols; x++) {
			this->particles.positions[y * cols + x] = pos;	
			this->particles.prev_positions[y * cols + x] = pos;	
			pos.x += spacing + this->particles.radius;
		}
		pos.x = origin.x;
		pos.y -= spacing + this->particles.radius;
	}
};

Solver::~Solver() { 
	delete this->spatial_grid;
 };

void Solver::update(const float _dt) {
	const uint32_t sub_steps = 8;	
	const glm::vec2 G(0.0f, -10.0f);
	const float dt = 0.01f / sub_steps;

	Solver::findNeighbours();	

	for (uint32_t s = 0; s < sub_steps; s++) {
		// Predict positions.
		for (uint32_t i = 0; i < this->particles.particle_count; i++) {
			this->particles.velocities[i] += G * dt;
			this->particles.prev_positions[i] = this->particles.positions[i];
			this->particles.positions[i] += this->particles.velocities[i] * dt;
		}

		// Solve constraints.
		Solver::solveBoundaries();
		Solver::solveFluid();

		for (uint32_t i = 0; i < this->particles.particle_count; i++) {
			glm::vec2 v = this->particles.positions[i] - this->particles.prev_positions[i];
			const float vel = glm::length(v);
			const float max_vel = 0.4f * this->particles.radius;
			
			// CFL. Essential for stablisation or everything will explode.
			if (vel > max_vel) {
				v *= max_vel / vel;
				this->particles.positions[i] = this->particles.prev_positions[i] + v;
			}
			this->particles.velocities[i] = v / dt;	
		}
	}
};

void Solver::findNeighbours() {
    this->spatial_grid->build();		
	for (uint32_t i = 0; i < this->particles.particle_count; i++) {
		const uint32_t query_size = this->spatial_grid->query(i);	
		this->particles.neighbour_counts[i] = query_size;		
	}
}

void Solver::solveBoundaries() {
	const float r = this->particles.radius;

	for (uint32_t i = 0; i < this->particles.particle_count; i++) {
		this->particles.positions[i].x = std::clamp(this->particles.positions[i].x, r, this->world_size.x - r);
		this->particles.positions[i].y = std::clamp(this->particles.positions[i].y, r, this->world_size.y - r);	
	}
}

void Solver::solveFluid() {
	const float eps = 0.0001f;
	const float particle_diameter = 2.0f * this->particles.radius;
	const float rest_density = 1.0f / (particle_diameter * particle_diameter);
	const float pi = 3.14159265f;
	const float h = this->smoothing_radius;
	const float h2 = h*h;
	const float kernel_scale = 4.0f / (pi * h2 * h2 * h2 * h2);

	for (uint32_t i = 0; i < this->particles.particle_count; i++) {	
		float rho = 0.0f;		
		float sum_grad2 = 0.0f;
		glm::vec2 grad_i(0.0f);

		const uint32_t query_size = this->particles.neighbour_counts[i];

		for (uint32_t j = 0; j < query_size; j++) {
			const uint32_t nb_i = this->particles.neighbours[i][j];
			glm::vec2 n = this->particles.positions[nb_i] - this->particles.positions[i];
			const float r2 = n.x*n.x + n.y*n.y;
						
			if (r2 > h2) {
				this->particles.gradients[nb_i] = glm::vec2(0.0f);
			} else {
				// Normalise.
				const float r = sqrt(r2);
				if (r > 0.0f) n /= r;

				const float w = h2 - r2;
				rho += kernel_scale * w * w * w;
				const float grad = (kernel_scale * 3.0f * w * w * (-2.0f * r)) / rest_density;
				this->particles.gradients[nb_i] = n * grad;
				grad_i -= n * grad;
				sum_grad2 += grad * grad;	
			}
		}		
		
		sum_grad2 += grad_i.x * grad_i.x + grad_i.y * grad_i.y;
		const float c = rho / rest_density - 1.0f;	
		const float lambda = -c / (sum_grad2 + eps);

		for (uint32_t j = 0; j < query_size; j++) {
			const uint32_t nb_i = this->particles.neighbours[i][j];
			if (i == nb_i) {
				this->particles.positions[nb_i] += lambda * grad_i;
			} else {
				this->particles.positions[nb_i] += lambda * this->particles.gradients[nb_i];
			}
		}	
	}	
}
