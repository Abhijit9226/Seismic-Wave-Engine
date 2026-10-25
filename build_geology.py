import numpy as np
import matplotlib.pyplot as plt

NX, NY = 500, 500
velocity = np.zeros((NX, NY), dtype=np.float32)

# Generate complex folded geology
for i in range(NX):
    for j in range(NY):
        depth = j
        # Layer 1: Surface sediments (folded)
        if depth < 100 + 20 * np.sin(i / 50.0):
            velocity[i, j] = 1500.0  
        # Layer 2: Sandstone 
        elif depth < 250 + 40 * np.sin(i / 80.0):
            velocity[i, j] = 2200.0  
        # Layer 3: Limestone (dipping)
        elif depth < 350 - 30 * np.cos(i / 60.0):
            velocity[i, j] = 2800.0  
        # Layer 4: Deep basement rock
        else:
            velocity[i, j] = 3500.0  

        # Feature: High-velocity Salt Dome intrusion
        dist_to_center = np.sqrt((i - 250)**2 + (j - 350)**2)
        if dist_to_center < 70:
            velocity[i, j] = 4500.0  

# Export raw binary data for the C engine
velocity.tofile('velocity_model.bin')

# Visualize the geology
plt.figure(figsize=(6, 6))
plt.imshow(velocity.T, cmap='jet')
plt.title("Synthetic Complex Geology (Salt Dome & Folds)")
plt.xlabel("X (meters)")
plt.ylabel("Depth (meters)")
plt.colorbar(label="Wave Velocity (m/s)")
plt.show()
