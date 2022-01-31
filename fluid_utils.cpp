#include <iostream>
#include <cstdlib>
#include "fluid.h"
#include "fluid_utils.h"

using namespace std;

int IX1(int x, int y)
{
    return x + y * N;
}


void lin_solve(int b, float* x, float* x0, float a, float c)
{
    float cRecip = 1.0 / c;
    for (int k = 0; k < iter; k++) {
        for (int j = 1; j < N - 1; j++) {
            for (int i = 1; i < N - 1; i++) {
                x[IX1(i, j)] =
                    (x0[IX1(i, j)]
                        + a * (x[IX1(i + 1, j)]
                            + x[IX1(i - 1, j)]
                            + x[IX1(i, j + 1)]
                            + x[IX1(i, j - 1)]
                            )) * cRecip;
            }
        }
        set_bnd(b, x);
    }
}


void diffuse(int b, float* x, float* x0, float diff, float dt)
{
    float a = dt * diff * (N - 2) * (N - 2);
    lin_solve(b, x, x0, a, 1 + 6 * a);
}

void project(float* velocX, float* velocY, float* p, float* div)
{
    for (int j = 1; j < N - 1; j++) {
        for (int i = 1; i < N - 1; i++) {
            div[IX1(i, j)] = -0.5f * (
                velocX[IX1(i + 1, j)]
                - velocX[IX1(i - 1, j)]
                + velocY[IX1(i, j + 1)]
                - velocY[IX1(i, j - 1)]
                ) / N;
            p[IX1(i, j)] = 0;
        }
    }

    set_bnd(0, div);
    set_bnd(0, p);
    lin_solve(0, p, div, 1, 6);


    for (int j = 1; j < N - 1; j++) {
        for (int i = 1; i < N - 1; i++) {
            velocX[IX1(i, j)] -= 0.5f * (p[IX1(i + 1, j)]
                - p[IX1(i - 1, j)]) * N;
            velocY[IX1(i, j)] -= 0.5f * (p[IX1(i, j + 1)]
                - p[IX1(i, j - 1)]) * N;

        }
    }

    set_bnd(1, velocX);
    set_bnd(2, velocY);

}


void set_bnd(int b, float* x)
{

    for (int i = 1; i < N - 1; i++) {
        x[IX1(i, 0)] = b == 2 ? -x[IX1(i, 1)] : x[IX1(i, 1)];
        x[IX1(i, N - 1)] = b == 2 ? -x[IX1(i, N - 2)] : x[IX1(i, N - 2)];
    }


    for (int j = 1; j < N - 1; j++) {
        x[IX1(0, j)] = b == 1 ? -x[IX1(1, j)] : x[IX1(1, j)];
        x[IX1(N - 1, j)] = b == 1 ? -x[IX1(N - 2, j)] : x[IX1(N - 2, j)];
    }


    x[IX1(0, 0)] = 0.5f * (x[IX1(1, 0)] + x[IX1(0, 1)]);
    x[IX1(0, N - 1)] = 0.5f * (x[IX1(1, N - 1)] + x[IX1(0, N - 2)]);
    x[IX1(N - 1, 0)] = 0.5f * (x[IX1(N - 2, 0)] + x[IX1(N - 1, 1)]);
    x[IX1(N - 1, N - 1)] = 0.5f * (x[IX1(N - 2, N - 1)] + x[IX1(N - 1, N - 2)]);

}


void advect(int b, float* d, float* d0, float* velocX, float* velocY, float dt)
{
    float i0, i1, j0, j1;

    float dtx = dt * (N - 2);
    float dty = dt * (N - 2);
    //float dtz = dt * (N - 2);

    float s0, s1, t0, t1;
    float tmp1, tmp2, x, y;

    float Nfloat = N - 2;
    float ifloat, jfloat;
    int i, j;


    for (j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
        for (i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
            tmp1 = dtx * velocX[IX1(i, j)];
            tmp2 = dty * velocY[IX1(i, j)];
            //tmp3 = dtz * velocZ[IX(i, j, k)];
            x = ifloat - tmp1;
            y = jfloat - tmp2;
            //z    = kfloat - tmp3;

            if (x < 0.5f) x = 0.5f;
            if (x > Nfloat + 0.5f) x = Nfloat + 0.5f;
            i0 = floorf(x);
            i1 = i0 + 1.0f;
            if (y < 0.5f) y = 0.5f;
            if (y > Nfloat + 0.5f) y = Nfloat + 0.5f;
            j0 = floorf(y);
            j1 = j0 + 1.0f;


            s1 = x - i0;
            s0 = 1.0f - s1;
            t1 = y - j0;
            t0 = 1.0f - t1;


            int i0i = i0;
            int i1i = i1;
            int j0i = j0;
            int j1i = j1;


            d[IX1(i, j)] =
                s0 * (t0 * d0[IX1(i0i, j0i)] + t1 * d0[IX1(i0i, j1i)]) +
                s1 * (t0 * d0[IX1(i1i, j0i)] + t1 * d0[IX1(i1i, j1i)]);
        }
    }

    set_bnd(b, d);
}

