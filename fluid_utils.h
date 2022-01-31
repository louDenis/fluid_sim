#pragma once

#ifndef FLUID_UTILS_H
#define FLUID_UTILS_H

void lin_solve(int b, float* x, float* x0, float a, float c);
void diffuse(int b, float* x, float* x0, float diff, float dt);
void project(float* velocX, float* velocY, float* p, float* div);
void set_bnd(int b, float* x);
void advect(int b, float* d, float* d0, float* velocX, float* velocY, float dt);

#endif