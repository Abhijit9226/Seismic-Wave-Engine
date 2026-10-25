import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

NX, NY = 500, 500
NT = 1500
NSHOTS = 5
DX = 10.0
DT = 0.001

try:
    survey_data = np.fromfile('survey_data.bin', dtype=np.float32)
    survey_data = survey_data.reshape((NSHOTS, NT, NX))

    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    extent_bounds = [0, NX * DX, NT * DT, 0]

    im1 = axes[0].imshow(survey_data[0], aspect='auto', cmap='seismic', 
                         vmin=-0.02, vmax=0.02, extent=extent_bounds)
    axes[0].set_title('Shot 1 Gather (Source at 1000m)')
    axes[0].set_xlabel('Distance Along Surface (meters)')
    axes[0].set_ylabel('Time (seconds)')
    fig.colorbar(im1, ax=axes[0], label='Vertical Wave Amplitude')

    im2 = axes[1].imshow(survey_data[4], aspect='auto', cmap='seismic', 
                         vmin=-0.02, vmax=0.02, extent=extent_bounds)
    axes[1].set_title('Shot 5 Gather (Source at 4000m)')
    axes[1].set_xlabel('Distance Along Surface (meters)')
    fig.colorbar(im2, ax=axes[1], label='Vertical Wave Amplitude')

    plt.tight_layout()
    plt.show() 

except FileNotFoundError:
    print("Error: 'survey_data.bin' not found.")

print("Loading animation for Shot 3...")
try:
    wave_data = np.fromfile('wave_snapshots.bin', dtype=np.float32)
    frames = len(wave_data) // (NX * NY)
    wave_data = wave_data.reshape((frames, NX, NY))

    fig2, ax2 = plt.subplots(figsize=(8, 8))
    im_anim = ax2.imshow(wave_data[0].T, cmap='seismic', vmin=-0.02, vmax=0.02, 
                         extent=[0, NX * DX, NY * DX, 0])
    ax2.set_title("Elastic Wave Propagation (Shot 3)")
    ax2.set_xlabel("Distance (meters)")
    ax2.set_ylabel("Depth (meters)")

    def update(frame):
        im_anim.set_array(wave_data[frame].T)
        return [im_anim]

    ani = animation.FuncAnimation(fig2, update, frames=frames, interval=20, blit=True)
    plt.show()

except FileNotFoundError:
    print("No animation file found.")
