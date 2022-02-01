#pragma once

#ifndef FLUID_H
#define FLUID_H

constexpr auto N = 32;
constexpr auto iter = 4;
constexpr auto SCALE = 8;
//int mouseX;
//int mouseY;


class Fluid
{
private:
    int size;
    float dt;
    float diff;
    float visc;

    float* s;
    float* density;

    float* Vx;
    float* Vy;
   
    float* Vx0;
    float* Vy0;

public:
    Fluid(float dt, float diffusion, float viscosity);
    float* array_init(float* array, int size);

    void step();

    void addDensity(int x, int y, float amount);
    void addVelocity(int x, int y, float amountX, float amountY);
    void FluidCubeFree();
    void renderD();
    float getDensity(int x, int y);

    //int getDensity(int mouseX, int mouseY);
    
};

#endif
