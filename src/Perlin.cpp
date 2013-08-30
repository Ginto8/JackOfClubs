#include "Perlin.hpp"
#include <cmath>
#include <cstdlib>
using namespace std;

double Perlin::_scurve(double t) {
    // f(t) = 6*t^5-15*t^4+10*t^3
    return t*t*t*(6*t*t-15*t+10);
}

Perlin::Perlin(int w,int h) {
    _w = w;
    int n = w*h;
    _grads.reserve(n);
    for(int i=0;i<n;++i) {
        int angle = rand()%360;
        _grads.push_back(Vec2d{{cos(angle*M_PI/180),sin(angle*M_PI/180)}});
    }
}

double Perlin::noise(double x,double y) {
    int w = _w;
    int h = _grads.size()/w;
    int x0 = ((int)x)%w,x1 = (x0+1)%w,
        y0 = ((int)y)%h,y1 = (y0+1)%h;
    double xFactor = x-(int)x,
          yFactor = y-(int)y;
    double n00 = dot(_grads[y0*w+x0],Vec2d{{xFactor,  yFactor}}),
          n10 = dot(_grads[y0*w+x1],Vec2d{{xFactor-1,yFactor}}),
          n01 = dot(_grads[y1*w+x0],Vec2d{{xFactor,  yFactor-1}}),
          n11 = dot(_grads[y1*w+x1],Vec2d{{xFactor-1,yFactor-1}});
    double sx = _scurve(xFactor),
          sy = _scurve(yFactor);
    double nx0 = n00*(1-sx)+n10*sx,
          nx1 = n01*(1-sx)+n11*sx;
    return nx0*(1-sy)+nx1*sy;
}

double Perlin::octaveNoise(double x,double y,double alpha,double beta,int n) {
    double scale = 1;
    double ret = 0;
    for(int i=0;i<n;++i) {
        ret += noise(x,y)/scale;
        x *= beta;
        y *= beta;
        scale *= alpha;
    }
    return ret;
}

