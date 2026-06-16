2D Seismic Wave Propagation Simulator

Overview-This project is a high-performance computational physics engine built from scratch in C. It models 2D acoustic wave propagation through heterogeneous geological media using the finite difference method. The simulator was developed to study seismic wave behavior—specifically transmission, reflection, and refraction (Snell's Law)—across distinct velocity boundaries, simulating real-world tectonic environments like fault lines and sedimentary basins.

Core Architecture-
1. Physics Engine: C (Compiled with GCC/MinGW)
2. Visualization: Python (matplotlib, numpy)
3. Mathematical Model: 2D Wave Equation discretized via the 5-point central finite difference stencil.
4. Memory Management: Dynamic heap allocation (calloc) to bypass stack limits, utilizing 1D array flattening to manage massive 2D spatial grids (1,000,000+ data      points per time step).

Technical Features-
1. Dynamic Courant-Friedrichs-Lewy (CFL) Calculation: The engine recalculates stability limits locally for every spatial coordinate based on the underlying           geological velocity model.
2. Heterogeneous Media Simulation: Supports custom velocity matrices ($c(x,y)$) to model varying rock densities.
3. Binary Data Pipeline: Exports raw 64-bit floating-point matrices to binary files (.bin) for high-speed I/O, allowing for seamless real-time animation rendering    in Python without memory bottlenecks.

Demonstration: Fault Line Refraction

<img width="381.2" height="338.8" alt="image" src="https://github.com/user-attachments/assets/31402952-d628-40af-8be0-f97af96fc009" />
<img width="381.2" height="338.8" alt="image" src="https://github.com/user-attachments/assets/18d555a7-c5f1-41bf-aa99-aa4947032d3d" />


https://github.com/user-attachments/assets/a7d873d2-9279-4c1a-b006-71e32afc66a9







Geological Model:

Left Hemisphere ($X < 250$): Dense Bedrock ($c = 4000$ m/s)

Right Hemisphere ($X \ge 250$): Soft Sedimentary Basin ($c = 2000$ m/s)

Observation: As the kinetic pulse expands from the epicenter, the wavefront traversing the sedimentary basin decelerates and compresses, resulting in observable wave refraction and energy trapping at the boundary layer.

Execution Instructions:
1. Compile the C engine: gcc wave2d.c -o wave2d -lm
2. Run the executable to generate the wave_animation.bin file.
3. Execute the visualization script: python animate.py
