#include "FftCorrelation.h"
#include "GrdCorrelation.h"
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#define PREPARG_FILE <ex_pl_args.h>
#include <PrepArg/usage.h>

#define PRINT( V ) qDebug() << #V " -> " << ( V )


QString img_name( QString pattern, int cpt ) {
    return QString( pattern ).arg( cpt, 3, 10, QChar( '0' ) );
}

struct MyShapefunctions {
    enum { nb_ddl = 5 };

    template<class T>
    void get( Vec<T,2> *sf, int x, int y ) const {
        sf[ 0 ][ 0 ] = 1.0;
        sf[ 0 ][ 1 ] = 0.0;

        sf[ 1 ][ 0 ] = 0.0;
        sf[ 1 ][ 1 ] = 1.0;

        sf[ 2 ][ 0 ] = ( x - xc ) / xc;
        sf[ 2 ][ 1 ] = 0;

        sf[ 3 ][ 0 ] = 0;
        sf[ 3 ][ 1 ] = ( y - yc ) / yc;

        if ( nb_ddl >= 5 ) {
            //            sf[ 4 ][ 0 ] = 0;
            //            sf[ 4 ][ 1 ] = pow( x - xc, 2 );
            sf[ 4 ][ 0 ] = y - yc;
            sf[ 4 ][ 1 ] = 0;
        }
    }
    double xc, yc;
};

Image<double> filter( const Image<double> &a, double fc ) {
    if ( not fc )
        return a;

    fftw_complex *fft_img_0 = (fftw_complex *)fftw_malloc( sizeof( fftw_complex ) * 2 * a.w * 2 * a.h );
    for( int y = 0, o = 0; y < 2 * a.h; ++y ) {
        for( int x = 0; x < 2 * a.w; ++x, ++o ) {
            fft_img_0[ o ][ 0 ] = a.val( x < a.w ? x : 2 * a.w - 1 - x, y < a.h ? y : 2 * a.h - 1 - y );
            fft_img_0[ o ][ 1 ] = 0;
        }
    }

    fftw_plan p = fftw_plan_dft_2d( 2 * a.h, 2 * a.w, fft_img_0, fft_img_0, FFTW_FORWARD, FFTW_ESTIMATE );
    fftw_execute_dft( p, fft_img_0, fft_img_0 );
    fftw_destroy_plan( p );

    for( int y = 0, o = 0; y < 2 * a.h; ++y ) {
        for( int x = 0; x < 2 * a.w; ++x, ++o ) {
            double rx = double( ( x + a.w ) % ( 2 * a.w ) - a.w ) / a.w;
            double ry = double( ( y + a.h ) % ( 2 * a.h ) - a.h ) / a.h;
            reinterpret_cast<std::complex<double> &>( fft_img_0[ o ] ) *= rx * rx + ry * ry < fc * fc;
        }
    }

    fftw_plan q = fftw_plan_dft_2d( 2 * a.h, 2 * a.w, fft_img_0, fft_img_0, FFTW_BACKWARD, FFTW_ESTIMATE );
    fftw_execute( q );
    fftw_destroy_plan( q );

    Image<double> r( a.w, a.h );
    for( int y = 0; y < a.h; ++y ) {
        for( int x = 0; x < a.w; ++x ) {
            int o = 2 * a.w * y + x;
            r.val( x, y ) = sqrt( pow( fft_img_0[ o ][ 0 ], 2 ) + pow( fft_img_0[ o ][ 1 ], 2 ) ); // fft_img_0[ o ][ 0 ]; // sqrt( pow( fft_img_0[ o ][ 0 ], 2 ) + pow( fft_img_0[ o ][ 1 ], 2 ) );
        }
    }

    r.contrasted( r.max() * 0.925, r.max() * 0.99, 0, 255 ).save( "filt.png" );
    return r;
}

int main( int argc, char **argv ) {
    // arguments
    #include <PrepArg/declarations.h>
    #include <PrepArg/parse.h>


    if ( not pattern )
        return usage( argv[ 0 ], "Please specify the pattern", 1 );

    try {
        Image<double> *m = 0;
        if ( mask )
            m = new Image<double>( mask );
        PRINT( flat_field );
        Image<double> f( flat_field );
        PRINT( img_name( pattern, 0 ) );
        Image<double> a( img_name( pattern, 0 ) ); a /= f;
        Image<double> filtered_a = filter( a, fc );
        x_max = qMin( x_max, a.w );
        y_max = qMin( y_max, a.h );
        a = a.slice( x_min, y_min, x_max, y_max );
        filtered_a = filtered_a.slice( x_min, y_min, x_max, y_max );

        if ( m )
            *m = m->slice( x_min, y_min, x_max, y_max );

        MyShapefunctions msf;
        msf.xc = ( x_max - x_min ) / 2;
        msf.yc = ( y_max - y_min ) / 2;

        // Save result in txt file
        QFile result_qfile( result_file );

        //Output in gnuplot graph
        FILE *plotpipe = popen( "gnuplot", "w" );
        fprintf( plotpipe, "set terminal x11 noraise\n" );
        fprintf( plotpipe, "set title \"Live 2D correlation\"\n" );
        fprintf( plotpipe, "set xlabel \"Scan number\"\n" );
        fprintf( plotpipe, "set ylabel \"Strain\"\n" );
        fprintf( plotpipe, "unset key\n" );

        fflush( plotpipe );


        // Stop file :file that stop the live correlation
        QString stopf = img_name( pattern, 9999 );
        bool stop = false;

        Vec<double,4> U_tot( 0 ); U_tot[ 0 ] = x_min; U_tot[ 1 ] = y_min;
        for( int cpt = 1; ; ++cpt ) {
            QString nf = img_name( pattern, cpt );
            for( bool first = true; ; ) {
                if ( QFile::exists( nf ) )
                    break;
                if ( QFile::exists( stopf ) ){
                    qDebug() << "Stop file found";
                    stop = true;
                    break;
                }
                if ( first ) {
                    first = false;
                    qDebug() << "Waiting for a new file...";
                }
                sleep( 1 );
            }

            if ( stop )
                break;

            Image<double> b( nf ); b /= f;
            Image<double> filtered_b = filter( b, fc );

            // pre-deformation of b (for the fft_correlation procedure)
            Image<double> d( x_max - x_min, y_max - y_min );

            for( int y = 0; y < d.h; ++y ) {
                for( int x = 0; x < d.w; ++x ) {
                    Vec<double,2> sf[ msf.nb_ddl ]; msf.get( sf, x, y );
                    double nx = x, ny = y;
                    for( int i = 0; i < msf.nb_ddl; ++i ) {
                        nx += U_tot[ i ] * sf[ i ][ 0 ];
                        ny += U_tot[ i ] * sf[ i ][ 1 ];
                    }
                    d.val( x, y ) = filtered_b.interp( qMax( 0.0, qMin( b.w - 2.0, nx ) ), qMax( 0.0, qMin( b.h - 2.0, ny ) ) );
                }
            }
            d.contrasted( d.min(), d.max(), 0, 255 ).save( QString( "flt_%1.png" ).arg( cpt, 3, 10, QChar( '0' ) ) );

            // first guess using fft
            //            int dx_0 = 0, dy_0 = 0;
            //            int dx_1 = 0, dy_1 = 0;
            //            int cx[] = { 0, a.w / 2, a.w };
            //            int cy[] = { 0, a.h / 2, a.h };
            //            fft_correlation( dx_0, dy_0, filtered_a.slice( cx[ 0 ], cy[ 0 ], cx[ 2 ], cy[ 1 ] ), d.slice( cx[ 0 ], cy[ 0 ], cx[ 2 ], cy[ 1 ] ) );
            //            fft_correlation( dx_1, dy_1, filtered_a.slice( cx[ 0 ], cy[ 1 ], cx[ 2 ], cy[ 2 ] ), d.slice( cx[ 0 ], cy[ 1 ], cx[ 2 ], cy[ 2 ] ) );
            //            U_tot[ 0 ] += ( dx_0 + dx_1 ) / 2;
            //            U_tot[ 1 ] += ( dy_0 + dy_1 ) / 2;
            //            U_tot[ 3 ] += ( dy_1 - dy_0 );
            //            qDebug() << "# -> " << dy_0 << dy_1 << U_tot[ 3 ];

            int dx_0 = 0, dy_0 = 0;
            fft_correlation( dx_0, dy_0, filtered_a, d );
            U_tot[ 0 ] += dx_0;
            U_tot[ 1 ] += dy_0;

            // corrections
            double dimensionless_residual;
            grd_correlation( d, U_tot, dimensionless_residual, msf, filtered_a, filtered_b, 0.1, 0.125, m );
            // grd_correlation( d, U_tot, dimensionless_residual, msf,          a,          b, 0.1, 0.125, m );

            // qDebug() << U_tot[ 3 ];

            // display residual
            //grd_correlation( d, U_tot, dimensionless_residual, msf, filtered_a, filtered_b, 1e3, 0.0, m );
            d.abs().contrasted( 0, d.abs().max(), 0, 255 ).save( QString( "img_%1.png" ).arg( cpt, 3, 10, QChar( '0' ) ) );

            // Save result in txt file
            if ( not result_qfile.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append ) )
                return 0;

            QTextStream flux( &result_qfile );
            flux.setCodec("UTF-8");
            flux << cpt << " " << U_tot[ 0 ] - x_min << " " << U_tot[ 1 ] - y_min << " " << U_tot[ 3 ] / msf.yc << " " << dimensionless_residual << endl;
            qDebug() << cpt << U_tot[ 0 ] - x_min << U_tot[ 1 ] - y_min << U_tot[ 3 ] / msf.yc << dimensionless_residual;
            result_qfile.close();

            fprintf( plotpipe, "plot \"%s\" u 1:4 w lp\n", result_file );
            //fprintf( plotpipe, "e\n" );
            fflush( plotpipe );

        }
        pclose( plotpipe );
        qDebug() << "Correlation finished";
    }
    catch ( const char *err ) { qDebug() << err; }
    catch ( QString err ) { qDebug() << err; }
}

