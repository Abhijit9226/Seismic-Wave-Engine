#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NX 500
#define NY 500
#define NT 1000

int main() {
    double dx = 10.0;
    double dt = 0.001;

    int total_points = NX * NY;
    double *u_past = (double *)calloc(total_points, sizeof(double));
    double *u_present = (double *)calloc(total_points, sizeof(double));
    double *u_future = (double *)calloc(total_points, sizeof(double));

    // --- The Velocity Matrix ---
    // This gives every single pixel on the grid its own speed limit
    double *c = (double *)calloc(total_points, sizeof(double));

    // --- Building the Fault Line ---
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            int pos = j * NX + i;
            if (i < NX / 2) {
                c[pos] = 4000.0; // Fast rock on the left half
            } else {
                c[pos] = 2000.0; // Slow sediment on the right half
            }
        }
    }

    // Inject the Earthquake exactly on the fault line
    int center_i = NX / 2;
    int center_j = NY / 2;
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            int pos = j * NX + i;
            double x_dist = (i - center_i) * dx;
            double y_dist = (j - center_j) * dx;
            u_present[pos] = exp(-(x_dist*x_dist + y_dist*y_dist) / 10000.0);
            u_past[pos] = u_present[pos];
        }
    }

    // Open binary file to save the animation frames
    FILE *fp = fopen("wave_animation.bin", "wb");
    int snapshot_interval = 10;

    // The Execution Loop
    for (int n = 0; n < NT; n++) {

        for (int j = 1; j < NY - 1; j++) {
            for (int i = 1; i < NX - 1; i++) {
                int pos   = j * NX + i;
                int left  = j * NX + (i - 1);
                int right = j * NX + (i + 1);
                int up    = (j - 1) * NX + i;
                int down  = (j + 1) * NX + i;

                // --- Dynamic Physics ---
                // The math now checks the specific speed of the ground right under it
                double C_local = (c[pos] * dt) / dx;

                u_future[pos] = 2.0 * u_present[pos] - u_past[pos] +
                                (C_local * C_local) * (u_present[right] - 2.0 * u_present[pos] + u_present[left] +
                                                       u_present[down]  - 2.0 * u_present[pos] + u_present[up]);
            }
        }

        // Save a frame every 10 steps
        if (n % snapshot_interval == 0) {
            fwrite(u_present, sizeof(double), total_points, fp);
        }

        // Move time forward
        for (int i = 0; i < total_points; i++) {
            u_past[i] = u_present[i];
            u_present[i] = u_future[i];
        }
    }

    fclose(fp);
    free(u_past);
    free(u_present);
    free(u_future);
    free(c); // Free the memory we used for the fault line

    printf("Heterogeneous Simulation complete. Binary file updated.\n");
    return 0;
}
