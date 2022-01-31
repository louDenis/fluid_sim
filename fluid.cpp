#include <iostream>
#include <cstdlib>
#include "fluid.h"
#include "fluid_utils.h"

//quand on clique, on addDensity a mousex mouseY
//fluid->addDensity(e->pos().x() / SCALE, e->pos().y() / SCALE, 10);

using namespace std;

int IX(int x, int y)
{
    if (x > N - 1)
        x = N - 1;
    if (x < 0)
        x = 0;
    if (y > N - 1)
        y = N - 1;
    if (y < 0)
        y = 0;
    return x + y * N;
}

float Fluid::getDensity(int x, int y)
{
    //std::cout << "get density-" << x << y << "=" << this->density[IX(x, y)] << std::endl;
    return this->density[IX(x, y)];
}

float* Fluid::array_init(float* array, int size)
{
    for (int i = 0; i < size * size; i++)
    {
        array[i] = 0;
        //std::cout << "array[" << i << "]=" << array[i] << std::endl;
    }

        
    return array;
}

Fluid::Fluid(float dt, float diffusion, float viscosity) {
    this->size = N;
    this->dt = dt;
    this->diff = diffusion;
    this->visc = viscosity;

    this->s = new float[size * size];
    this->s = array_init(this->s, this->size);
    
    this->density = new float[size * size];
    this->density = array_init(this->density, this->size);

    this->Vx = new float[size * size];
    this->Vx = array_init(this->Vx, this->size);

    this->Vy = new float[size * size];
    this->Vy = array_init(this->Vy, this->size);

    //cube->Vz = calloc(N * N * N, sizeof(float));

    this->Vx0 = new float[size * size];
    this->Vx0 = array_init(this->Vx0, this->size);

    this->Vy0 = new float[size * size];
    this->Vy0 = array_init(this->Vy0, this->size);
    //cube->Vz0 = calloc(N * N * N, sizeof(float));
}

void Fluid::step()
{
    //std::cout << "------DEBUT STEP----" << std::endl;
    int N= this->size;
    float visc = this->visc;
    float diff = this->diff;
    float dt = this->dt;
    float* Vx = this->Vx;
    float* Vy = this->Vy;
    //float *Vz      = cube->Vz;
    float* Vx0 = this->Vx0;
    float* Vy0 = this->Vy0;
    //float *Vz0     = cube->Vz0;
    float* s = this->s;
    float* density = this->density;

    diffuse(1, Vx0, Vx, visc, dt);
    diffuse(2, Vy0, Vy, visc, dt);
    //diffuse(3, Vz0, Vz, visc, dt, 4, N);

    project(Vx0, Vy0, Vx, Vy);

 
    advect(1, Vx, Vx0, Vx0, Vy0, dt);
    advect(2, Vy, Vy0, Vx0, Vy0, dt);
    //advect(3, Vz, Vz0, Vx0, Vy0, Vz0, dt, N);

    project(Vx, Vy, Vx0, Vy0);

    diffuse(0, s, density, diff, dt);
    advect(0, density, s, Vx, Vy, dt);
}

void Fluid::addDensity(int x, int y, float amount)
{
    int index = IX(x, y);
    this->density[IX(x, y)] += amount;
    std::cout << "density/" << x << "," << y << "=" << this->density[IX(x, y)] << std::endl;
    //std::cout << "addDensity :" << this->density[IX(x, y)] << std::endl;
}

void Fluid::addVelocity(int x, int y, float amountX, float amountY)
{
    int index = IX(x, y);
    this->Vx[index] += amountX;
    this->Vy[index] += amountY;
    std::cout << "velocity x/" << x << "," << y << "=" << this->Vx[index] << std::endl;
    std::cout << "velocity y/" << x << "," << y << "=" << this->Vy[index] << std::endl;

}

void Fluid::FluidCubeFree()
{
    free(this->s);
    free(this->density);

    free(this->Vx);
    free(this->Vy);
    //free(cube->Vz);

    free(this->Vx0);
    free(this->Vy0);
    //free(cube->Vz0);

    free(this);
}

void Fluid::renderD()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            float x = i * SCALE;
            float y = j * SCALE;
            float d = this->density[IX(i, j)];
            //couleur 
            //draw pixel x y scale
        }
    }
}


//dessiner a mouseX, mouseY, SCALE, SCALE, avec la couleur d= getDenisty correspondant a mouseX, mouseY






