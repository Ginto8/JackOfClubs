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
    // later, when used...
    Maybe<T> val = f();
    if(val && val.is()) { // equivalent boolean expressions
        // these two are equivalent
        T data = val;
        T data2 = val.get();
        // do stuff
    } else {
        // Each of these throws std::logic_error.
        // Monad<T>'s data is only well-defined if val.is() == true
        T data = val;
        T data2 = val.get();
    }
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
    const T& get() const {
        if(!_val) {
            throw std::logic_error("Monad::get() requires "\
                                   "Monad::is() == true");
        }
        return *_val;
    }

    explicit operator bool() const {
        return is();
    }
    explicit operator T() const {
        return get();
    }
};

#endif
