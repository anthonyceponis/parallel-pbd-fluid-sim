#include <glm/glm.hpp>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cmath>

#include "constants.hpp"

__global__ void fluidKernel(glm::vec2 *d_positions, glm::vec2 *d_gradients, uint32_t *d_neighbours, uint32_t *d_neighbour_counts, const uint32_t n) {
	uint32_t i = threadIdx.x + blockIdx.x * blockDim.x;
	if (i >= n) return;

	float rho = 0.0f;		
	float sum_grad2 = 0.0f;
	glm::vec2 grad_i(0.0f);

	const uint32_t query_size = d_neighbour_counts[i];

	for (uint32_t j = 0; j < query_size; j++) {
		const uint32_t nb_i = d_neighbours[i * n + j];
		glm::vec2 n = d_positions[nb_i] - d_positions[i];
		const float r2 = glm::dot(n,n);
					
		if (r2 > constants::h2) {
			d_gradients[nb_i] = glm::vec2(0.0f);
		} else {
			// Normalise.
			const float r = sqrt(r2);
			if (r > 0.0f) n /= r;

			const float w = constants::h2 - r2;
			rho += constants::kernel_scale * w * w * w;
			const float grad = (constants::kernel_scale * 3.0f * w * w * (-2.0f * r)) / constants::rest_density;
			d_gradients[nb_i] = n * grad;
			grad_i -= n * grad;
			sum_grad2 += grad * grad;	
		}
	}		
	
	sum_grad2 += glm::dot(grad_i,grad_i);
	const float c = rho / constants::rest_density - 1.0f;	
	const float lambda = -c / (sum_grad2 + constants::eps);

	for (uint32_t j = 0; j < query_size; j++) {
		const uint32_t nb_i = d_neighbours[i * n + j];
		if (i == nb_i) {
			d_positions[nb_i] += lambda * grad_i;
		} else {
			d_positions[nb_i] += lambda * d_gradients[nb_i];
		}
	}
}

void solveFluidGpu(glm::vec2 *h_positions, glm::vec2 *h_gradients, uint32_t *h_neighbours, uint32_t *h_neighbour_counts, const uint32_t n) {
	// Allocate memory on device.	
	glm::vec2 *d_positions;
	glm::vec2 *d_gradients;
	uint32_t *d_neighbours; // This 2d array is flattened.
	uint32_t *d_neighbour_counts;

	cudaMalloc((void**)&d_positions, n * sizeof(glm::vec2));
	cudaMalloc((void**)&d_gradients, n * sizeof(glm::vec2));
	cudaMalloc((void**)&d_neighbours, n * n * sizeof(uint32_t));
	cudaMalloc((void**)&d_neighbour_counts, n * sizeof(uint32_t));
	
	// Copy data from host to device.	
	cudaMemcpy(d_positions, h_positions, n * sizeof(glm::vec2), cudaMemcpyHostToDevice);
	cudaMemcpy(d_gradients, h_gradients, n * sizeof(glm::vec2), cudaMemcpyHostToDevice);
	cudaMemcpy(d_neighbours, h_neighbours , n * n * sizeof(uint32_t), cudaMemcpyHostToDevice);
	cudaMemcpy(d_neighbours, h_neighbour_counts, n * sizeof(uint32_t), cudaMemcpyHostToDevice);
	
	uint32_t threads_per_block = 256;
	uint32_t blocks_per_grid = (n + threads_per_block - 1) / threads_per_block;
	
	fluidKernel<<<blocks_per_grid, threads_per_block>>>(d_positions, d_gradients, d_neighbours, d_neighbour_counts, n);

	// Copy results from device to host.
	cudaMemcpy(h_positions, d_positions, n * sizeof(glm::vec2), cudaMemcpyDeviceToHost);
	cudaMemcpy(h_gradients, d_gradients, n * sizeof(glm::vec2), cudaMemcpyDeviceToHost);
	cudaMemcpy(h_neighbours, d_neighbours, n * n * sizeof(uint32_t), cudaMemcpyDeviceToHost);
	cudaMemcpy(h_neighbour_counts, d_neighbour_counts, n * sizeof(uint32_t), cudaMemcpyDeviceToHost);
	
	cudaFree(d_positions);
	cudaFree(d_gradients);
	cudaFree(d_neighbours);
	cudaFree(d_neighbour_counts);
}
