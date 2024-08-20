# Position Based Fluid Implementation
This project is a 2D implementation of https://mmacklin.com/pbf_sig_preprint.pdf

I used C++ and OpenGL to render over 10,000 particles at 8 substeps on a single thread at 30 fps on my local machine. I also experimented with higher numbers, utilising parallel GPU processing with CUDA, but ran into skill issues regarding stabilisation. 

### Running the sim
```bash
bash run.sh
```

### Preview
