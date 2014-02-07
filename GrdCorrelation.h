#ifndef GRDCORRELATION_H
#define GRDCORRELATION_H

#include "Image.h"
#include "Mat.h"

template<class I,class T,class Q,class SF>
bool grd_iteration( Image<I> &r, T &contrast, Q &U, T &dimensionless_residual, const SF &sl, const Image<I> &f, const Image<I> &g, T conv, T relax, const Image<I> *mask = 0 ) {
    const int nb_ddl = SF::nb_ddl;
    Mat<T,nb_ddl> M( 0 );
    Vec<T,nb_ddl> V( 0 );
    int nb_pix = 0;

    for( int n = 0; n < nb_ddl; ++n )
        M( n, n ) += 1e-16;

    dimensionless_residual = 0;
    T mean_f = 0, mean_g = 0;
    for( I y_f = 0; y_f < f.h; ++y_f ) {
        for( I x_f = 0; x_f < f.w; ++x_f ) {
            r.val( x_f, y_f ) = 0;

            T x_g = x_f;
            T y_g = y_f;
            Vec<T,2> s[ nb_ddl ];
            sl.get( s, x_f, y_f );
            for( int i = 0; i < nb_ddl; ++i ) {
                x_g += s[ i ][ 0 ] * U[ i ];
                y_g += s[ i ][ 1 ] * U[ i ];
            }
            if ( y_g < 0 or y_g + 1 >= g.h or x_g < 0 or x_g + 1 >= g.w )
                continue;

            T ori_f = f.val( x_f, y_f );
            T val_f = contrast * ori_f;

            T val_g = g.interp( x_g, y_g );
            T grx_g = g.grad_x( x_g, y_g );
            T gry_g = g.grad_y( x_g, y_g );

            r.val( x_f, y_f ) = val_f - val_g;
            if ( mask and mask->val( x_f, y_f ) < 127 ) {
                r.val( x_f, y_f ) *= 0.125;
                continue;
            }

            T sf[ nb_ddl ];
            for( int i = 0; i < nb_ddl; ++i ) {
                sf[ i ] = s[ i ][ 0 ] * grx_g + s[ i ][ 1 ] * gry_g;

                V[ i ] += sf[ i ] * ( val_f - val_g );
                for( int j = 0; j <= i; ++j )
                    M( i, j ) += sf[ i ] * sf[ j ];
            }

            ++nb_pix;
            mean_f += ori_f;
            mean_g += val_g;

            dimensionless_residual += pow( val_f - val_g, 2 );
        }
    }

    contrast = mean_g / mean_f;

    M.chol();
    Vec<T,nb_ddl> D = M.solve_using_cholesky( V );

    for( int i = 0; i < nb_ddl; ++i )
        U[ i ] += relax * D[ i ];
    dimensionless_residual = sqrt( dimensionless_residual / nb_pix );

    // qDebug() << "# " << D[ 0 ] << D[ 1 ] << D[ 2 ] << D[ 3 ] << " r=" << dimensionless_residual << conv;
    for( int i = 0; i < nb_ddl; ++i )
        if ( fabs( D[ i ] ) > conv )
            return true;
    return false;
}


template<class I,class V,class T,class SF>
void grd_correlation( Image<I> &r, V &U, T &dimensionless_residual, const SF &sf, const Image<I> &a, const Image<I> &b, T conv, T relax = 1.0, const Image<I> *mask = 0 ) {
    T contrast = 1;
    for( int cpt_iter = 0; ; ++cpt_iter ) {
        if ( cpt_iter == 500 ) {
            qDebug() << "# convergence pb";
            return;
        }
        if ( not grd_iteration( r, contrast, U, dimensionless_residual, sf, a, b, conv, relax, mask ) )
            break;
    }
}

#endif // GRDCORRELATION_H
