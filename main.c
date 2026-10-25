#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define NX 500
#define NY 500
#define NT 1500
#define DX 10.0
#define DT 0.001
#define NSHOTS 5
#define SNAPSHOT 10

float ux[NX][NY], ux_prev[NX][NY], ux_next[NX][NY];
float uz[NX][NY], uz_prev[NX][NY], uz_next[NX][NY];
float vp[NX][NY];
float vs[NX][NY];
float damp[NX][NY];

int main() {
    FILE *vel_file = fopen("velocity_model.bin", "rb");
    if (vel_file == NULL) {
        printf("Error: Run build_geology.py first!\n");
        return 1;
    }
    fread(vp, sizeof(float), NX * NY, vel_file);
    fclose(vel_file);

    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            vs[i][j] = vp[i][j] / 1.732;
        }
    }

    int PAD = 40;
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            damp[i][j] = 1.0;
            int dist_x = (i < NX / 2) ? i : NX - 1 - i;
            int dist_y = (j < NY / 2) ? j : NY - 1 - j;

            if (dist_x < PAD) {
                float depth = (float)(PAD - dist_x) / PAD;
                damp[i][j] *= exp(-0.15 * depth * depth);
            }
            if (dist_y < PAD) {
                float depth = (float)(PAD - dist_y) / PAD;
                damp[i][j] *= exp(-0.15 * depth * depth);
            }
        }
    }

    FILE *shot_file = fopen("survey_data.bin", "wb");
    FILE *wave_file = fopen("wave_snapshots.bin", "wb");
    int rec_y = 10;

    for (int shot = 0; shot < NSHOTS; shot++) {
        for (int i = 0; i < NX; i++) {
            for (int j = 0; j < NY; j++) {
                ux[i][j] = 0.0; ux_prev[i][j] = 0.0; ux_next[i][j] = 0.0;
                uz[i][j] = 0.0; uz_prev[i][j] = 0.0; uz_next[i][j] = 0.0;
            }
        }

        int source_x = 100 + (shot * 75);
        printf("Firing shot %d of %d at X = %d meters...\n", shot + 1, NSHOTS, source_x * (int)DX);

        for (int t = 0; t < NT; t++) {
            float time_sec = t * DT;

            #pragma omp parallel for
            for (int i = 1; i < NX - 1; i++) {
                for (int j = 1; j < NY - 1; j++) {
                    float d2ux_dx2 = (ux[i+1][j] - 2.0*ux[i][j] + ux[i-1][j]) / (DX*DX);
                    float d2ux_dz2 = (ux[i][j+1] - 2.0*ux[i][j] + ux[i][j-1]) / (DX*DX);
                    float d2ux_dxdz = (ux[i+1][j+1] - ux[i+1][j-1] - ux[i-1][j+1] + ux[i-1][j-1]) / (4.0*DX*DX);

                    float d2uz_dx2 = (uz[i+1][j] - 2.0*uz[i][j] + uz[i-1][j]) / (DX*DX);
                    float d2uz_dz2 = (uz[i][j+1] - 2.0*uz[i][j] + uz[i][j-1]) / (DX*DX);
                    float d2uz_dxdz = (uz[i+1][j+1] - uz[i+1][j-1] - uz[i-1][j+1] + uz[i-1][j-1]) / (4.0*DX*DX);

                    float vp2 = vp[i][j] * vp[i][j];
                    float vs2 = vs[i][j] * vs[i][j];

                    ux_next[i][j] = 2.0*ux[i][j] - ux_prev[i][j] + DT*DT * (vp2 * d2ux_dx2 + vs2 * d2ux_dz2 + (vp2 - vs2) * d2uz_dxdz);
                    uz_next[i][j] = 2.0*uz[i][j] - uz_prev[i][j] + DT*DT * (vp2 * d2uz_dz2 + vs2 * d2uz_dx2 + (vp2 - vs2) * d2ux_dxdz);
                }
            }

            float f = 10.0;
            float t0 = 1.0 / f;
            float arg = M_PI * f * (time_sec - t0);
            uz_next[source_x][NY/4] += (1.0 - 2.0 * arg * arg) * exp(-arg * arg);

            #pragma omp parallel for
            for (int i = 0; i < NX; i++) {
                for (int j = 0; j < NY; j++) {
                    ux_next[i][j] *= damp[i][j]; ux[i][j] *= damp[i][j];
                    uz_next[i][j] *= damp[i][j]; uz[i][j] *= damp[i][j];
                }
            }

            float surface_row[NX];
            for (int i = 0; i < NX; i++) {
                surface_row[i] = uz_next[i][rec_y];
            }
            fwrite(surface_row, sizeof(float), NX, shot_file);

            if (shot == 2 && t % SNAPSHOT == 0) {
                fwrite(uz_next, sizeof(float), NX * NY, wave_file);
            }

            #pragma omp parallel for
            for (int i = 0; i < NX; i++) {
                for (int j = 0; j < NY; j++) {
                    ux_prev[i][j] = ux[i][j]; ux[i][j] = ux_next[i][j];
                    uz_prev[i][j] = uz[i][j]; uz[i][j] = uz_next[i][j];
                }
            }
        }
    }

    fclose(shot_file);
    fclose(wave_file);
    printf("\nSurvey complete. Data saved.\n");
    return 0;
}
