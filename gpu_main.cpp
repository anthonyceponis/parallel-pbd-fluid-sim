#include <glm/glm.hpp>
#include <vector>
#include <cstdint>


void solveFluidGpu(glm::vec2 *h_positions, glm::vec2 *h_gradients, uint32_t *h_neighbours, uint32_t *h_neighbour_counts, const uint32_t n); 


int main() {
	std::vector<glm::vec2> positions = {glm::vec2(1,1)};	
	std::vector<glm::vec2> gradients = {glm::vec2(1,1)};	
	std::vector<uint32_t> neighbours = {0};
	std::vector<uint32_t> neighbour_counts = {0};

	solveFluidGpu(positions.data(), gradients.data(), neighbours.data(), neighbour_counts.data(), 1);	

	return 0;
		
}
