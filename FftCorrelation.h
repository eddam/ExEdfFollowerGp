#ifndef FFTCORRELATION_H
#define FFTCORRELATION_H

#include "Image.h"
#include <fftw3.h>
#include <limits>

#ifdef METIL_COMP_DIRECTIVE
#pragma lib_name fftw3
#endif

inline int next_pow_2( int a ) {
    for( int i = 1; ; i *= 2 )
        if ( i >= a )
            return i;
}

inline int prev_pow_2( int a ) {
    for( int i = 1; ; i *= 2 )
        if ( i > a )
            return i / 2;
}

template<class TI>
void fft_correlation( int &dx, int &dy, const TI &img_0, const TI &img_1 ) {
    typedef typename TI::B T;

    int sx = prev_pow_2( img_0.w );
    int sy = prev_pow_2( img_0.h );

    fftw_complex *fft_img_0 = (fftw_complex *)fftw_malloc( sizeof( fftw_complex ) * sx * sy );
    fftw_complex *fft_img_1 = (fftw_complex *)fftw_malloc( sizeof( fftw_complex ) * sx * sy );
    for( int y = 0, o = 0; y < sy; ++y ) {
        for( int x = 0; x < sx; ++x, ++o ) {
            fft_img_0[ o ][ 0 ] = img_0.val( x - sx / 2 + img_0.w / 2, y - sy / 2 + img_0.h / 2 ); fft_img_0[ o ][ 1 ] = 0;
            fft_img_1[ o ][ 0 ] = img_1.val( x - sx / 2 + img_1.w / 2, y - sy / 2 + img_1.h / 2 ); fft_img_1[ o ][ 1 ] = 0;
        }
    }

    fftw_plan p = fftw_plan_dft_2d( sy, sx, fft_img_0, fft_img_0, FFTW_FORWARD, FFTW_ESTIMATE );
    fftw_execute_dft( p, fft_img_0, fft_img_0 );
    fftw_execute_dft( p, fft_img_1, fft_img_1 );
    fftw_destroy_plan( p );

    for( int y = 0, o = 0; y < sy; ++y )
        for( int x = 0; x < sx; ++x, ++o )
            reinterpret_cast<std::complex<double> &>( fft_img_1[ o ] ) *=
                conj( reinterpret_cast<std::complex<double> &>( fft_img_0[ o ] ) );

    fftw_plan q = fftw_plan_dft_2d( sy, sx, fft_img_1, fft_img_1, FFTW_BACKWARD, FFTW_ESTIMATE );
    fftw_execute( q );
    fftw_destroy_plan( q );


    double best = - std::numeric_limits<double>::max();
    for( int y = - sy / 4; y <= sy / 4; ++y ) {
        for( int x = - sx / 4; x <= sx / 4; ++x ) {
            int o = sx * ( ( y + sy ) % sy ) + ( x + sx ) % sx;
            if ( best < fft_img_1[ o ][ 0 ] ) {
                best = fft_img_1[ o ][ 0 ];
                dx = x;
                dy = y;
            }
        }
    }

    fftw_free( fft_img_1 );
    fftw_free( fft_img_0 );
}

#endif // FFTCORRELATION_H
