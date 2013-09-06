#ifndef NUMUTIL_HPP
#define NUMUTIL_HPP

template<class T>
T signum(T x) {
    return x > 0 ?  1 :
           x < 0 ? -1 :
                    0;
}

#endif
