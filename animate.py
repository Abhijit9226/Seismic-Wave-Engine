import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Grid parameters (Must match the C code exactly)
NX = 500
NY = 500

print("Loading Binary Data...")
# Load the raw binary 1s and 0s and interpret them as 64-bit floats (doubles in C)
raw_data = np.fromfile('wave_animation.bin', dtype=np.float64)

# Calculate how many frames we saved
num_frames = len(raw_data) // (NX * NY)
print(f"Successfully loaded {num_frames} frames of animation.")

# Reshape the massive 1D array back into a 3D matrix (Frames, Y, X)
frames = raw_data.reshape((num_frames, NY, NX))

print("Initializing Render Engine...")
# Build the UI
fig, ax = plt.subplots(figsize=(8, 8))
ax.set_title("2D Acoustic Wavefront Animation", fontsize=14, fontweight='bold')
ax.set_xlabel("Spatial Grid X")
ax.set_ylabel("Spatial Grid Y")

# Render the first frame. 
# We lock vmin and vmax so the color scale doesn't violently flash every frame.
im = ax.imshow(frames[0], cmap='seismic', vmin=-0.03, vmax=0.03, interpolation='bilinear', origin='lower')
fig.colorbar(im, label="Wave Amplitude")

# The Animation Function (This runs in a loop)
def update(frame_idx):
    # Swap out the data for the next frame
    im.set_array(frames[frame_idx])
    return [im]

# Stitch it together
print("Playing Animation...")
ani = animation.FuncAnimation(fig, update, frames=num_frames, interval=30, blit=True)

plt.show()
