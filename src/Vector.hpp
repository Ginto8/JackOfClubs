#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <functional>
#include <ostream>

template<class T,int N>
struct Vector {
    static const Vector ZERO;

    T values[N];

    template<class U>
    operator Vector<U,N>() const {
        Vector<U,N> ret;
        for(int i=0;i<N;++i) {
            ret[i] = (U)values[i];
        }
        return ret;
    }

    bool operator==(const Vector& v) const {
        for(int i=0;i<N;++i) {
            if(values[i] != v[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Vector& v) const {
        return !(*this == v);
    }

    Vector& operator +=(const Vector& v) {
        for(int i=0;i<N;++i) {
            values[i] += v[i];
        }
        return *this;
    }
    Vector& operator -=(const Vector& v) {
        for(int i=0;i<N;++i) {
            values[i] -= v[i];
        }
        return *this;
    }
    Vector& operator *=(T s) {
        for(int i=0;i<N;++i) {
            values[i] *= s;
        }
        return *this;
    }
    Vector& operator /=(T s) {
        for(int i=0;i<N;++i) {
            values[i] /= s;
        }
        return *this;
    }
    Vector operator+(const Vector& v) const {
        Vector ret = *this;
        return (ret += v);
    }
    Vector operator-(const Vector& v) const {
        Vector ret = *this;
        return (ret -= v);
    }
    Vector operator*(T s) const {
        Vector ret = *this;
        return (ret *= s);
    }
    Vector operator/(T s) const {
        auto ret = *this;
        return (ret/=s);
    }
    Vector operator-() const {
        Vector ret;
        for(int i=0;i<N;++i) {
            ret[i] = -values[i];
        }
        return ret;
    }

    T& operator[](unsigned n) {
        return values[n];
    }
    const T& operator[](unsigned n) const {
        return values[n];
    }

    T magSquared() const {
        T ret = 0;
        for(auto x: values) {
            ret += x*x;
        }
        return ret;
    }
    T mag() const {
        return std::sqrt(this->magSquared());
    }
};

template<class T,class U,int N>
Vector<T,N> convert(Vector<U,N> v) {
    Vector<T,N> ret;
    for(int i=0;i<N;++i) {
        ret[i] = (T)v[i];
    }
    return ret;
}

template<class T,int N>
const Vector<T,N> Vector<T,N>::ZERO = Vector<T,N>();

template<class T,int N>
Vector<T,N> operator*(T s,const Vector<T,N>& v) {
    return v*s;
}

template<class T,int N>
Vector<T,N> norm(const Vector<T,N>& v) {
    return v/v.mag();
}
template<class T,int N>
T dot(const Vector<T,N>& a,const Vector<T,N>& b) {
    T ret = 0;
    for(int i=0;i<N;++i) {
        ret += a[i]*b[i];
    }
    return ret;
}

template<class T = float>
using Vec2 = Vector<T,2>;
template<class T = float>
using Vec3 = Vector<T,3>;
template<class T = float>
using Vec4 = Vector<T,4>;

template<class T>
Vec3<T> cross(const Vec3<T>& a,const Vec3<T>& b) {
    return { a[1]*b[2]-a[2]*b[1],
             a[2]*b[0]-a[0]*b[2],
             a[0]*b[1]-a[1]*b[0] };
}

namespace std {
    template<class T,int N>
    struct hash<Vector<T,N>> {
        static size_t rotate(size_t x,int n) {
            if(n == 0) {
                return x;
            }
            return (x<<n | x>>(sizeof(x)*8-n));
        }
        size_t operator()(const Vector<T,N>& v) const {
            size_t result = 0;
            for(int i=0;i<N;++i) {
                result ^= rotate(hash<T>()(v[i]),i);
            }
            return result;
        }
    };
    template<class T,int N>
    ostream& operator<<(ostream& o,const Vector<T,N>& v) {
        o << "Vec" << N << "{";
        bool first = true;
        for(auto x:v.values) {
            if(!first) {
                o << ",";
            }
            first = false;
            o << x;
        }
        o << "}";
        return o;
    }

}

using Vec2i = Vec2<int>;
using Vec3i = Vec3<int>;
using Vec3i = Vec3<int>;
using Vec4i = Vec4<int>;
using Vec2f = Vec2<float>;
using Vec3f = Vec3<float>;
using Vec4f = Vec4<float>;
using Vec2d = Vec2<double>;
using Vec3d = Vec3<double>;
using Vec4d = Vec4<double>;

template<class T = float>
using Color3 = Vec3<T>;
template<class T = float>
using Color4 = Vec4<T>;

using Color3f = Color3<float>;
using Color4f = Color4<float>;

constexpr Color3f RED   = { 1,0,0 };
constexpr Color3f GREEN = { 0,1,0 };
constexpr Color3f BLUE  = { 0,0,1 };
constexpr Color3f BLACK = { 0,0,0 };
constexpr Color3f WHITE = { 1,1,1 };

#endif
