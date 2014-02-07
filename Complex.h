#ifndef COMPLEX_H
#define COMPLEX_H

#include <complex>

template<class I>
struct SubType {
    typedef I T;
    static T extract( I val ) { return val; }
};

template<class I>
struct SubType<std::complex<I> > {
    typedef I T;
    static T extract( I val ) { return std::abs( val ); }
};

#endif // COMPLEX_H
