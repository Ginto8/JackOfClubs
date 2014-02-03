#ifndef MAYBE_HPP
#define MAYBE_HPP

#include <stdexcept>
#include <memory>

/* Implementation of the "Maybe" monad. Acts like a boolean + optional extra
   data.
   Usage:
    Maybe<T> f() {
        if(!someCondition) {
            return {};
        }
        T calculatedData;
        // calculate data...
        return {calculatedData};
    }
    // later...
    Maybe<T> val = f();
    T x;
    // these two have the same effect
    if(val.extract(x)) {
        // do stuff with x
    }
    if(val) {
        val.forward([&x](const T& v) {
            x = v;
        });
        // do stuff with x
    }
    // Or if you don't have to use x afterwards:
    val.forward([](const T& x) {
        // do stuff with x
    });
 */
template<class T>
class Maybe {
    std::shared_ptr<T> _val;
public:
    Maybe() = default;
    Maybe(const Maybe& other) : _val(other._val) {}
    Maybe(T val) : _val(new T(val)) {}

    bool is() const {
        return (bool)_val;
    }
    bool extract(T& out) const {
        if(_val) {
            out = *_val;
            return true;
        }
        return false;
    }
    void forward(std::function<void(T)> f) const {
        if(_val) {
            f(*_val);
        }
    }

    explicit operator bool() const {
        return is();
    }
};

#endif
