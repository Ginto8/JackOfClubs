#include "Vector.hpp"
#include <iostream>

template<class T,int N>
void f(Vector<T,N> v) {
    std::cout << "Vec" << N << "{";
    bool first = true;
    for(auto x:v.values) {
        if(!first) {
            std::cout << ",";
        }
        first = false;
        std::cout << x;
    }
    std::cout << "}" << std::endl;
}

int main() {
    auto v = Vec3f();
    f(v);
    auto v1 = Vec3f({3,2,1});
    f(v1);
    f(Vec4d({5,3,2,1}));
    return 0;
}
