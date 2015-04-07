#include "Color.hpp"

#include <iostream>

Color3f rgbToHsv(Color3f c) {
    /* for(unsigned i = 0; i < 3; ++i) { */
    /*     if(c[i] > 1) { */
    /*         c[i] = 1; */
    /*     } else if(c[i] < 0) { */
    /*         c[i] = 0; */
    /*     } */
    /* } */

    float h,s,v;
    size_t maxIndex = c[0] >= c[1] && c[0] >= c[2] ? 0 :
                      c[1] >= c[2]                 ? 1 :
                                                     2;
    size_t nextInd = (maxIndex+1)%3,
           lastInd = (maxIndex+2)%3;
    size_t minIndex = c[nextInd] <= c[lastInd] ? nextInd : lastInd;
    float maxVal = c[maxIndex],
          minVal = c[minIndex];
    v = maxVal;
    if(maxVal == 0 || maxVal == minVal) {
        s = 0;
        h = 0;
    } else {
        s = 1 - (minVal/maxVal);
        float delta = maxVal - minVal;
        size_t nextInd = (maxIndex+1)%3,
               lastInd = (maxIndex+2)%3;
        h = (c[nextInd]-c[lastInd])/delta + (maxIndex*2);
        if(h < 0) {
            h = 6;
        }
        h /= 6;
    }

    /* std::cout << "RGB(" << c[0] << "," << c[1] << "," << c[2] */ 
    /*           << ") -> HSV(" << h << "," << s << "," << v << ")" */
    /*           << std::endl; */

    return {{ h,s,v }};
}

Color3f hsvToRgb(Color3f c) {
    float h = c[0],
          s = c[1],
          v = c[2];
    /* if(s < 0) { */
    /*     s = 0; */
    /* } else if(s > 1) { */
    /*     s = 1; */
    /* } */
    /* if(v < 0) { */
    /*     v = 0; */
    /* } else if(v > 1) { */
    /*     v = 1; */
    /* } */
    /* h = std::fmod(h,1); */
    /* if(h < 0) { */
    /*     h += 1; */
    /* } */

    float mainVal = v*s,
          secondVal = mainVal*(1-std::fabs(std::fmod(6*h,2)-1)),
          white = v*(1-s);

    int range = std::floor(6*h);
    int mainIndex = ((range+1)%6)/2;
    int secondOffset = (range%2 == 0 ? 1 : -1);
    int secondIndex = (mainIndex + secondOffset)%3;
    if(secondIndex < 0) {
        secondIndex += 3;
    }

    Color3f ret = {{ white,white,white }};
    ret[mainIndex] += mainVal;
    ret[secondIndex] += secondVal;

    /* std::cout << "HSV(" << c[0] << "," << c[1] << "," << c[2] */ 
    /*           << ") -> RGB(" << ret[0] << "," << ret[1] << "," */ 
    /*           << ret[2] << ")" << std::endl; */

    return ret;
}

Color3f hsvInterpolate(Color3f a,Color3f b,float factor,float base) {
    if(a[1] == 0) {
        a[0] = base;
    }
    if(b[1] == 0) {
        b[0] = base;
    }
    a[0] -= base;
    b[0] -= base;
    if(a[0] < 0) {
        a[0] += 1;
    }
    if(b[0] < 0) {
        b[0] += 1;
    }

    auto diff = b-a;
    auto ret = a + factor*diff;
    ret[0] = std::fmod(ret[0] + base,1.0f);
    if(ret[0] < 0) {
        ret[0] += 1;
    }
    return ret;
}

// Circular interpolation. Not what I'm looking for from the seawifs
// data.
/* Color3f hsvInterpolate(Color3f a,Color3f b,float factor) { */
/*     auto diff = b-a; */
/*     float absHueDiff = std::fabs(diff[0]); */

/*     // wrap hue values around */
/*     if(absHueDiff > 0.5) { */
/*         float sign = diff[0] < 0 ? 1 : -1; */
/*         diff[0] = sign*(1 - absHueDiff); */
/*     } */
/*     auto c = a + factor*diff; */
/*     if(c[0] < 0) { */
/*         c[0] += 1; */
/*     } */

/*     /1* if(b[1] == 0) { *1/ */
/*     /1*     c[0] = a[0]; *1/ */
/*     /1* } else if(a[1] == 0) { *1/ */
/*     /1*     c[0] = b[0]; *1/ */
/*     /1* } *1/ */

/*     return c; */
/* } */

Color3f seawifsFilterColor(Color3f c) {
    if(c[1] < 0.05) {
        c[1] = 0;
    }
    // Eliminate overlay on land (sat < 0.1 value > 0.1)
    if(c[1] == 0) {
        c[0] = 5.0/6;
        c[2] = (c[2] >= 0.46 ? 0.5 : 0);
    }
    // Eliminate overlay on sea (saturation > 0.5)
    if(c[1] > 0.1) {
        c[1] = 1;
    }
    return c;
}

