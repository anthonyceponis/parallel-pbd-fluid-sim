nvcc -O3 --use_fast_math -c  solver_gpu.cu -o kernel.o
g++ -c -Ofast main.cpp glad.c shader.cpp particles.cpp spatial_grid.cpp renderer.cpp -Iinclude -ldl -lglfw -o main.o
nvcc main.o kernel.o -o a.out
./a.out
