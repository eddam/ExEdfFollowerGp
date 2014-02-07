#ifndef VEC_H
#define VEC_H

template<unsigned num> struct Number {};
template<class T> struct TensorOrder { static const int res = 0; };
template<class T,int s> class Vec;
struct Size {};
#define ASSERT_IF_DEBUG( A )

template<class T,int s>
struct TensorOrder<Vec<T,s> > {
    static const int res = 1;
};


template<class ItemType,int _static_size>
class Vec {
public:
    static const int static_size = _static_size;
    typedef ItemType T;
    typedef int ST;

    Vec() {}

    template<class T0>
    void _init_using_1_arg( const T0 &v0, Number<0> ) {
        for(int i=0;i<static_size;++i)
            _data[ i ] = v0;
    }

    template<class T0>
    void _init_using_1_arg( const T0 &v0, Number<1> ) {
        ASSERT_IF_DEBUG( v0.size() == static_size_ );
        for(int i=0;i<static_size;++i)
            _data[ i ] = v0[ i ];
    }

    template<class T0>
    Vec( const T0 &v0 ) {
        _init_using_1_arg( v0, Number<TensorOrder<T0>::res>() );
    }

    template<class T0,class T1>
    Vec( const T0 &v0, const T1 &v1 ) {
        ASSERT_IF_DEBUG( static_size_ == 2 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
    }

    template<class T0,class T1,class T2>
    Vec( const T0 &v0, const T1 &v1, const T2 &v2 ) {
        ASSERT_IF_DEBUG( static_size_ == 3 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
        _data[ 2 ] = v2;
    }

    template<class T0,class T1,class T2,class T3>
    Vec( const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3 ) {
        ASSERT_IF_DEBUG( static_size_ == 4 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
        _data[ 2 ] = v2;
        _data[ 3 ] = v3;
    }

    template<class T0,class T1,class T2,class T3,class T4>
    Vec( const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4 ) {
        ASSERT_IF_DEBUG( static_size_ == 5 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
        _data[ 2 ] = v2;
        _data[ 3 ] = v3;
        _data[ 4 ] = v4;
    }

    template<class T0,class T1,class T2,class T3,class T4,class T5>
    Vec( const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5 ) {
        ASSERT_IF_DEBUG( static_size_ == 6 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
        _data[ 2 ] = v2;
        _data[ 3 ] = v3;
        _data[ 4 ] = v4;
        _data[ 5 ] = v5;
    }

    template<class T0,class T1,class T2,class T3,class T4,class T5,class T6>
    Vec( const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5 , const T6 &v6) {
        ASSERT_IF_DEBUG( static_size_ == 7 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
        _data[ 2 ] = v2;
        _data[ 3 ] = v3;
        _data[ 4 ] = v4;
        _data[ 5 ] = v5;
        _data[ 6 ] = v6;
    }

    template<class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    Vec( const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5 , const T6 &v6, const T7 &v7) {
        ASSERT_IF_DEBUG( static_size_ == 8 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
        _data[ 2 ] = v2;
        _data[ 3 ] = v3;
        _data[ 4 ] = v4;
        _data[ 5 ] = v5;
        _data[ 6 ] = v6;
        _data[ 7 ] = v7;
    }

    template<class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    Vec( const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5 , const T6 &v6, const T7 &v7, const T8 &v8) {
        ASSERT_IF_DEBUG( static_size_ == 9 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
        _data[ 2 ] = v2;
        _data[ 3 ] = v3;
        _data[ 4 ] = v4;
        _data[ 5 ] = v5;
        _data[ 6 ] = v6;
        _data[ 7 ] = v7;
        _data[ 8 ] = v8;
    }

    template<class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
    Vec( const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5 , const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9) {
        ASSERT_IF_DEBUG( static_size_ == 10 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
        _data[ 2 ] = v2;
        _data[ 3 ] = v3;
        _data[ 4 ] = v4;
        _data[ 5 ] = v5;
        _data[ 6 ] = v6;
        _data[ 7 ] = v7;
        _data[ 8 ] = v8;
        _data[ 9 ] = v9;
    }

    template<class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10>
    Vec( const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5 , const T6 &v6, const T7 &v7, const T8 &v8, const T9 &v9, const T10 &v10) {
        ASSERT_IF_DEBUG( static_size_ == 11 );
        _data[ 0 ] = v0;
        _data[ 1 ] = v1;
        _data[ 2 ] = v2;
        _data[ 3 ] = v3;
        _data[ 4 ] = v4;
        _data[ 5 ] = v5;
        _data[ 6 ] = v6;
        _data[ 7 ] = v7;
        _data[ 8 ] = v8;
        _data[ 9 ] = v9;
        _data[ 10 ] = v10;
    }

    Vec( Size, int s ) {
        ASSERT_IF_DEBUG( s == static_size );
    }

    template<class T0>
    Vec( Size, int s, const T0 &v0 ) {
        ASSERT_IF_DEBUG( s == static_size );
        for(int i=0;i<static_size;++i)
            _data[ i ] = v0;
    }

    Vec( const Vec &v ) {
        ASSERT_IF_DEBUG( static_size_ == v.size() );
        for(int i=0;i<static_size;++i)
            _data[ i ] = v[ i ];
    }

    void operator=( const Vec &v ) {
        ASSERT_IF_DEBUG( static_size_ == v.size() );
        for(int i=0;i<static_size;++i)
            _data[ i ] = v[ i ];
    }

    template<class T0>
    void operator=( const T0 &v0 ) {
        _init_using_1_arg( v0, Number<TensorOrder<T0>::res>() );
    }

    void set( T val ) {
        for( ST i = 0; i < static_size; ++i )
            _data[ i ] = val;
    }

    void operator+=( const Vec &v ) {
        for( int i = 0; i < static_size; ++i )
            _data[ i ] += v[ i ];
    }


    int size() const {
        return static_size;
    }

    int rese() const {
        return static_size;
    }

    const T &operator[]( int i ) const {
        ASSERT_IF_DEBUG( i < static_size );
        return _data[ i ];
    }

    T &operator[]( int i ) {
        ASSERT_IF_DEBUG( i < static_size );
        return _data[ i ];
    }

    bool empty() const { return size() == 0; }

    void resize( int new_size ) {
        ASSERT_IF_DEBUG( new_size == static_size_ );
    }

    void resize( int new_size, const T &val ) {
        ASSERT_IF_DEBUG( new_size == static_size_ );
    }

    void reserve( int new_rese ) {
        ASSERT_IF_DEBUG( new_rese == static_size_ );
    }

    T *begin() { return _data; }
    T *end() { return _data + static_size; }
    const T *begin() const { return _data; }
    const T *end() const { return _data + static_size; }

    T *ptr() { return _data; }
    const T *ptr() const { return _data; }

    T &back() { return _data[ static_size - 1 ]; }
    const T &back() const { return _data[ static_size - 1 ]; }

    template<class T2>
    bool contains( const T2 &d ) const {
        for( int i = 0; i < static_size; ++i )
            if ( _data[ i ] == d )
                return true;
            return false;
    }

    bool operator!=( const Vec &v ) const {
        for( int i = 0; i < static_size; ++i )
            if ( _data[ i ] != v[ i ] )
                return true;
            return false;
    }

    bool operator==( const Vec &v ) const {
        for( int i = 0; i < static_size; ++i )
            if ( _data[ i ] != v[ i ] )
                return false;
            return true;
    }

    bool operator==( const T &v ) const {
        for(int i=0;i<static_size;++i)
            if ( _data[ i ] != v )
                return false;
            return true;
    }


    Vec operator+( const Vec &vec ) const {
        Vec res;
        for(int i=0;i<static_size;++i)
            res[ i ] = operator[]( i ) + vec[ i ];
        return res;
    }

    template<class TS>
    void write_str( TS &os ) const {
        if ( size() ) {
            os << operator[]( 0 );
            for(ST i=1;i<size();++i) {
                os.write_separator( 0 );
                os <<  operator[]( i );
            }
        }
    }
private:
    T _data[ static_size ];
};

#endif // VEC_H
