#ifndef MAT44_HPP
#define MAT44_HPP

#include <cmath>
#include "Vector.hpp"

template<class T>
struct Mat44 {
    static Mat44 ZERO;
    static Mat44 IDENT;

    static Mat44 translation(T x,T y,T z);
    template<class U>
    static Mat44 translation(const Vec3<U>& v);
    static Mat44 rotation(T degAngle,T x,T y,T z);
    template<class U>
    static Mat44 rotation(T degAngle,const Vec3<U>& axis);
    static Mat44 orthoProjection(T left,  T right,
                                 T bottom,T top,
                                 T near,  T far);
    static Mat44 frustrumProjection(T left,  T right,
                                    T bottom,T top,
                                    T near,  T far);
    static Mat44 perspectiveProjection(T degFovy,
                                       T aspectRatio,
                                       T near,T far);

    T values[16];

    Mat44& operator+=(const Mat44& other) {
        for(unsigned i=0;i<16;++i) {
            values[i] += other[i];
        }
    }
    Mat44& operator-=(const Mat44& other) {
        for(unsigned i=0;i<16;++i) {
            values[i] -= other[i];
        }
    }
    Mat44& operator*=(T s) {
        for(unsigned i=0;i<16;++i) {
            values[i] *= s;
        }
    }

    Mat44 operator+(const Mat44& other) {
        Mat44 ret = *this;
        return (ret += other);
    }
    Mat44 operator-(const Mat44& other) {
        Mat44 ret = *this;
        return (ret -= other);
    }
    Mat44 operator*(T s) {
        Mat44 ret = *this;
        return (ret *= s);
    }
    Mat44 operator-() {
        Mat44 ret;
        for(unsigned i=0;i<16;++i) {
            ret[i] = -values[i];
        }
        return ret;
    }

    T& operator[](unsigned i) {
        return values[i];
    }
    const T& operator[](unsigned i) const {
        return values[i];
    }
};

template<class T>
Mat44<T> Mat44<T>::ZERO = { 0,0,0,0,
                            0,0,0,0,
                            0,0,0,0,
                            0,0,0,0 };
template<class T>
Mat44<T> Mat44<T>::IDENT = { 1,0,0,0,
                             0,1,0,0,
                             0,0,1,0,
                             0,0,0,1 };

template<class T>
Mat44<T> operator*(T s,const Mat44<T>& m) {
    return m*s;
}

template<class T>
Mat44<T> operator*(const Mat44<T>& a,const Mat44<T>& b) {
    Mat44<T> ret;
    for(int c=0;c<4;++c) {
        for(int r=0;r<4;++r) {
            T val = 0;
            for(int i=0;i<4;++i) {
                val += a[i*4+r]*b[c*4+i];
            }
            ret[c*4+r] = val;
        }
    }
    return ret;
}

template<class T>
Vec4<T> operator*(const Mat44<T>& A,const Vec4<T>& v) {
    Vec4<T> ret;
    for(int r=0;r<4;++r) {
        T val = 0;
        for(int i=0;i<4;++i) {
            val += A[i*4+r]*v[i];
        }
        ret[r] = val;
    }
    return ret;
}

template<class T>
Vec4<T> operator*(const Mat44<T>& A,const Vec3<T>& v) {
    return A*Vec4<T>{{ v[0],v[1],v[2],1 }};
}

template<class T>
Mat44<T> transpose(const Mat44<T>& m) {
    Mat44<T> ret;
    for(int c=0;c<4;++c) {
        for(int r=0;r<4;++r) {
            ret[c*4+r] = m[r*4+c];
        }
    }
    return ret;
}

template<class T>
Mat44<T> Mat44<T>::translation(T x,T y,T z) {
    // column-major, so this looks like the transpose of what it
    // actually is.
    return {{ 1,0,0,0,
              0,1,0,0,
              0,0,1,0,
              x,y,z,1 }};
}

template<class T>
template<class U>
Mat44<T> Mat44<T>::translation(const Vec3<U>& v) {
    return Mat44<T>::translation(v[0],v[1],v[2]);
}

template<class T>
Mat44<T> Mat44<T>::rotation(T degAngle,
                            T x,T y,T z) {
    double radAngle = degAngle*M_PI/180;
    T s = std::sin(radAngle),c = std::cos(radAngle);
    return {{ x*x*(1-c)+c,
              x*y*(1-c)+z*s,
              x*z*(1-c)-y*s,
              0,
 
              x*y*(1-c)-z*s,
              y*y*(1-c)+c,
              y*z*(1-c)+x*s,
              0,

              x*z*(1-c)+y*s,
              y*z*(1-c)-x*s,
              z*z*(1-c)+c,
              0,
                     
              0,
              0,
              0,
              1 }};
}

template<class T>
template<class U>
Mat44<T> Mat44<T>::rotation(T degAngle,const Vec3<U>& v) {
    return Mat44<T>::rotation(degAngle,v[0],v[1],v[2]);
}

template<class T>
Mat44<T> Mat44<T>::orthoProjection(T left,  T right,
                                   T bottom,T top,
                                   T near,  T far) {
    Mat44<T> ret = { 2/(right-left),
                     0,
                     0,
                     0,
                  
                     0,
                     2/(top-bottom),
                     0,
                     0,

                     0,
                     0,
                     -2/(far-near),
                     0,

                     -(right+left)/(right-left),
                     -(top+bottom)/(top-bottom),
                     -(near+far)/(near-far),
                     1 };
    return ret;
}

template<class T>
Mat44<T> Mat44<T>::frustrumProjection(T left,  T right,
                                      T bottom,T top,
                                      T near,  T far) {
    Mat44<T> ret{{ 2*near/(right-left),
                   0,
                   0,
                   0,

                   0,
                   2*near/(top-bottom),
                   0,
                   0,

                   (right+left)/(right-left),
                   (top+bottom)/(top-bottom),
                   (near+far)/(near-far),
                   -1,
                   
                   0,
                   0,
                   2*far*near/(near-far),
                   0 }};
    return ret;
}

template<class T>
Mat44<T> Mat44<T>::perspectiveProjection(T degFovy,
                                         T aspectRatio,
                                         T near,T far) {
    T radFovy = degFovy*M_PI/180;
    T halfVert = near*std::tan(radFovy/2);
    T halfHoriz = halfVert*aspectRatio;
    return Mat44<T>::frustrumProjection(-halfHoriz,halfHoriz,
                                        -halfVert, halfVert,
                                        near,      far);
}

using Mat44f = Mat44<float>;
using Mat44d = Mat44<double>;

#endif
