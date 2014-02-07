#ifndef MAT_H
#define MAT_H

#include "Vec.h"

template<class T_,int dim>
struct Mat {
    typedef T_ T;
    static const int size = dim * ( dim + 1 ) / 2;

    Mat() {}
    Mat( T val ) : data( val ) {}

    int nb_rows() const { return dim; }
    int nb_cols() const { return dim; }

    const T &sec_sel( int r, int c ) const { return data[ r * ( r + 1 ) / 2 + c ]; } ///< assuming c <= r
    T &sec_sel( int r, int c ) { return data[ r * ( r + 1 ) / 2 + c ]; } ///< assuming c <= r

    const T &operator()( int r, int c ) const { return r < c ? sec_sel( c, r ) : sec_sel( r, c ); }
    T &operator()( int r, int c ) { return r < c ? sec_sel( c, r ) : sec_sel( r, c ); }

    void operator+=( const Mat &m ) {
        data += m.data;
    }

    void operator=( const T &v ) {
        data = v;
    }

    void set( T val ) { data.set( val ); }


    void complete_diag( T c = 0 ) {
        for( int i = 0; i < dim; ++i )
            operator()( i, i ) += operator()( i, i ) == 0;
        if ( c ) {
            T m = 0;
            for( int i = 0; i < dim; ++i )
                m = max( m, abs( operator()( i, i ) ) );
            for( int i = 0; i < dim; ++i )
                operator()( i, i ) += c * m;
        }
    }


    void chol( int d = dim ) {
        for( int r = 0; r < d; ++r ) {
            for( int c = 0; c < r; ++c ) {
                T val = sec_sel( r, c );
                for( int i = 0; i < c; ++i )
                    val -= sec_sel( r, i ) * sec_sel( c, i );
                sec_sel( r, c ) = val * sec_sel( c, c );
            }
            T val = sec_sel( r, r );
            for( int i = 0; i < r; ++i )
                val -= pow( sec_sel( r, i ), 2 );
            sec_sel( r, r ) = 1 / sqrt( val );
        }

    }

    Vec<T,dim> solve_using_cholesky( Vec<T,dim> sol, int d = dim ) const {
        Vec<T,dim> res;
        for( int r = 0; r < d; ++r ) {
            T v = sol[ r ];
            for( int c = 0; c < r; ++c )
                v -= sec_sel( r, c ) * res[ c ];
            res[ r ] = v * sec_sel( r, r );
        }

        for( int r = d- 1; r >= 0; --r ) {
            T v = res[ r ];
            for( int c = r + 1; c < d; ++c )
                v -= sec_sel( c, r ) * res[ c ];
            res[ r ] = v * sec_sel( r, r );
        }

        return res;
    }

    void add_l_Id( T l ) {
        for( int c = 0; c < dim; ++c )
            sec_sel( c, c ) += l;
    }

    Vec<T,dim> operator*( const Vec<T,dim> &vec ) const {
        Vec<T,dim> res( 0 );
        for( int r = 0; r < dim; ++r )
            for( int c = 0; c < dim; ++c )
                res[ r ] += operator()( r, c ) * vec[ c ];
        return res;
    }

    template<class TS>
    void write_str( TS &os ) const {
        for( int r = 0; r < dim; ++r ) {
            for( int c = 0; c < dim; ++c )
                os << ( c ? " " : "" ) << operator()( r, c );
            os << "\n";
        }
    }

    Vec<T,size> data;
};

#endif // MAT_H
