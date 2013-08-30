#ifndef PERLIN_HPP
#define PERLIN_HPP

#include "Vector.hpp"
#include <vector>

// I believe this is a half-decent Perlin noise interpretation
class Perlin {
    static double _scurve(double t);

    std::vector<Vec2d> _grads;
    int _w;
public:
    Perlin(int w,int h);

    double noise(double x,double y);
    double octaveNoise(double x,double y,double alpha=2,double beta=2,int n=6);
};

#endif
