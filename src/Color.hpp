#ifndef COLOR_HPP
#define COLOR_HPP

#include <SFML/Graphics/Color.hpp>
#include "Vector.hpp"

inline bool isNearlyBlack(sf::Color c,uint8_t thresh = 20) {
    return c.r < thresh && c.g < thresh && c.b < thresh;
}

inline uint32_t color2uint(sf::Color c) {
    return (((uint32_t)c.r)<<(3*8)) |
           (((uint32_t)c.g)<<(2*8)) |
           (((uint32_t)c.b)<<(1*8)) |
           (((uint32_t)c.a)<<(0*8));
}

inline sf::Color uint2color(uint32_t i) {
    sf::Color c;
    c.r = (i>>(3*8))&0xFF;
    c.g = (i>>(2*8))&0xFF;
    c.b = (i>>(1*8))&0xFF;
    c.a = (i>>(0*8))&0xFF;
    return c;
}

inline Color4f fromSfColor(sf::Color c) {
    return {{ c.r/255.0f,c.g/255.0f,
              c.b/255.0f,c.a/255.0f }};
}

inline sf::Color toSfColor(Color4f c) {
    return sf::Color(std::round(c[0]*255),std::round(c[1]*255),
                     std::round(c[2]*255),std::round(c[3]*255));
}

template<typename T>
Color4<T> c3ToC4(Color3<T> c,T a) {
    return {{ c[0],c[1],c[2],a }};
}

template<typename T>
Color3<T> c4ToC3(Color4<T> c) {
    return {{ c[0],c[1],c[2] }};
}

Color3f rgbToHsv(Color3f c);

Color3f hsvToRgb(Color3f c);

Color3f hsvInterpolate(Color3f a,Color3f b,float factor,
                       float base=5.0/6);

Color3f seawifsFilterColor(Color3f c);

#endif
