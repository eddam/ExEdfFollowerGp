#ifndef IMAGE_H
#define IMAGE_H

#include <QtCore/QTextStream>
#include <QtCore/QVector>
#include <QtCore/QRegExp>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QImage>
#include "Complex.h"

#ifdef METIL_COMP_DIRECTIVE
#pragma fra_name QtGui
#pragma fra_name QtCore
#pragma inc_path /usr/include/qt4
#endif

/**
*/
template<class T>
struct Image {
    typedef typename SubType<T>::T ST; // subtype (e.g. for complex)
    typedef T B;

    Image( int w = 0, int h = 0 ) : w( w ), h( h ), data( w * h ) {}

    Image( QString fn ) {
        if ( fn.endsWith( ".edf" ) )
            load_edf( fn );
        else
            load_qt_img( fn );
    }

    void resize( int w, int h ) { // hum...
        this->data.resize( w * h );
        this->w = w;
        this->h = h;
    }

    void load_edf( QString fn ) {
        QFile file( fn );
        if ( not file.open( QFile::ReadOnly ) )
            throw "Impossible to open file.";
        QByteArray file_data = file.readAll();

        QTextStream text_stream( &file_data );
        QRegExp d1( "Dim_1.*=.*(\\d+)" );
        QRegExp d2( "Dim_2.*=.*(\\d+)" );
        int dim_1 = 0, dim_2 = 0;
        while ( true ) {
            QString line = text_stream.readLine();
            if ( text_stream.status() != QTextStream::Ok ) {
                throw "Corrupted .edf file.";
                return;
            }

            // header
            if ( d1.indexIn( line ) >= 0 )
                dim_1 = d1.cap( 1 ).toInt();
            if ( d2.indexIn( line ) >= 0 )
                dim_2 = d2.cap( 1 ).toInt();

            // begin of data
            int e = line.indexOf( '}' );
            if ( e >= 0 ) {
                resize( dim_1, dim_2 );
                for( int y = 0, o = text_stream.pos(); y < h; ++y )
                    for( int x = 0; x < w; ++x, o += 2 )
                        val( x, y ) = uchar( file_data.at( o + 0 ) ) + ( ushort( uchar( file_data.at( o + 1 ) ) ) << 8 );
                break;
            }
        }
    }

    void load_qt_img( QString fn ) {
        QImage img; img.load( fn );
        if ( img.depth() == 0 )
            throw "Failed to load img '" + fn + "'";

        w = img.width();
        h = img.height();
        data.resize( w * h );

        const uchar *ptr = img.bits();
        if ( img.depth() == 8 ) {
            for( int y = 0; y < h; ++y )
                for( int x = 0; x < w; ++x )
                    data[ w * y + x ] = ptr[ y * img.bytesPerLine() + x ];
        } else if ( img.depth() == 32 ) {
            for( int y = 0; y < h; ++y )
                for( int x = 0; x < w; ++x )
                    data[ w * y + x ] = 0.333333333333333 * (
                        ptr[ y * img.bytesPerLine() + 4 * x + 0 ] +
                        ptr[ y * img.bytesPerLine() + 4 * x + 1 ] +
                        ptr[ y * img.bytesPerLine() + 4 * x + 2 ]
                    );
        } else {
            throw "img.depth() not supported ";
        }

    }

    void save( QString filename ) const {
        QImage img( w, h, QImage::Format_ARGB32 );
        uchar *ptr = img.bits();
        for( int i = 0; i < w * h; ++i, ptr += 4 ) {
            ptr[ 0 ] = qMax( T( 0 ), qMin( T( 255 ), data[ i ] ) );
            ptr[ 1 ] = qMax( T( 0 ), qMin( T( 255 ), data[ i ] ) );
            ptr[ 2 ] = qMax( T( 0 ), qMin( T( 255 ), data[ i ] ) );
            ptr[ 3 ] = 255;
        }
        img.save( filename );
    }

    int display( QString filename = "tmp.png" ) const {
        save( filename );
        return system( ( "display " + filename ).toAscii().data() );
    }

    Image contrasted( T mi, T ma, T fmi, T fma ) const {
        Image res( w, h );
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                res.val( x, y ) = fmi + ( fma - fmi ) * ( val( x, y ) - mi ) / ( ma - mi );
        return res;
    }

    Image contrasted( T mi, T ma ) const {
        return contrasted( min(), max(), mi, ma );
    }

    Image slice( int x_0, int y_0, int x_1, int y_1 ) const {
        Image res( x_1 - x_0, y_1 - y_0 );
        for( int y = y_0, dy = 0; y < y_1; ++y, ++dy )
            for( int x = x_0, dx = 0; x < x_1; ++x, ++dx )
                res.val( dx, dy ) = val( x, y );
        return res;
    }

    void normalize() {
        T m = mean();
        T s = std_dev();
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                val( x, y ) = ( val( x, y ) - m ) / s;
    }


    T min() const {
        T res = data[ 0 ];
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                res = qMin( res, val( x, y ) );
        return res;
    }

    T max() const {
        T res = data[ 0 ];
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                res = qMax( res, val( x, y ) );
        return res;
    }

    T mean() const {
        T res = 0;
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                res += val( x, y );
        return res / ( w * h );
    }

    T std_dev() const {
        T res = 0, m = mean();
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                res += pow( val( x, y ) - m, 2 );
        return sqrt( res / ( w * h ) );
    }

    Image<ST> abs() const {
        Image<ST> res( w, h );
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                res.val( x, y ) = std::abs( val( x, y ) );
        return res;
    }

    Image log() const {
        Image res( w, h );
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                res.val( x, y ) = std::log( val( x, y ) );
        return res;
    }

    Image filtered( T c_1 ) const {
        Image res( w, h );
        T c_0 = 1 - c_1;

        // +x
        for( int y = 0; y < h; ++y ) {
            T v = val( 0, y );
            for( int x = 0; x < w; ++x ) {
                v = c_0 * v + c_1 * val( x, y );
                res.val( x, y ) = v;
            }
        }

        // -x
        for( int y = 0; y < h; ++y ) {
            T v = val( w - 1, y );
            for( int x = w - 1; x >= 0; --x ) {
                v = c_0 * v + c_1 * res.val( x, y );
                res.val( x, y ) = v;
            }
        }

        // +y
        for( int x = 0; x < w; ++x ) {
            T v = val( x, 0 );
            for( int y = 0; y < h; ++y ) {
                v = c_0 * v + c_1 * res.val( x, y );
                res.val( x, y ) = v;
            }
        }

        // -y
        for( int x = 0; x < w; ++x ) {
            T v = val( x, h - 1 );
            for( int y = h - 1; y >= 0; --y ) {
                v = c_0 * v + c_1 * res.val( x, y );
                res.val( x, y ) = v;
            }
        }

        return res;
    }

    template<class U>
    void operator/=( const Image<U> &d ) {
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                val( x, y ) /= d.val( x, y );
    }

    const T &val( int x, int y ) const { return data[ w * y + x ]; }
    T &val( int x, int y ) { return data[ w * y + x ]; }

    template<class V>
    T interp( V x, V y ) const {
        V x_f = x; int x_i = x_f; x_f -= x_i;
        V y_f = y; int y_i = y_f; y_f -= y_i;

        return ( 1 - y_f ) * ( 1 - x_f ) * val( x_i + 0, y_i + 0 ) +
               ( 1 - y_f ) * ( 0 + x_f ) * val( x_i + 1, y_i + 0 ) +
               ( 0 + y_f ) * ( 1 - x_f ) * val( x_i + 0, y_i + 1 ) +
               ( 0 + y_f ) * ( 0 + x_f ) * val( x_i + 1, y_i + 1 );

    }

    T grad_x( int x, int y ) const {
        return val( x + 1, y + 0 ) - val( x + 0, y + 0 );
    }

    T grad_y( int x, int y ) const {
        return val( x + 0, y + 1 ) - val( x + 0, y + 0 );
    }

    T grad_x( int x, int y, int s ) const {
        return val( x + s, y + 0 ) - val( x + 0, y + 0 );
    }

    T grad_y( int x, int y, int s ) const {
        return val( x + 0, y + s ) - val( x + 0, y + 0 );
    }

    const T *ptr() const {
        return data.data();
    }

    T *ptr() {
        return data.data();
    }

    Image<T> box_filter( int c ) const {
        Image<float> tmp( w / c, h / c );
        for( int y = 0; y < h / c; ++y ) {
            for( int x = 0; x < w / c; ++x ) {
                T s = 0, v = 0;
                for( int dy = c * y + 0; dy < std::min( h, c * y + c ); ++dy ) {
                    for( int dx = c * x + 0; dx < std::min( w, c * x + c ); ++dx ) {
                        v += val( dx, dy );
                        ++s;
                    }
                }
                tmp.val( x, y ) = v / s;
            }
        }

        Image<T> res( w, h );
        for( int y = 0; y < h; ++y )
            for( int x = 0; x < w; ++x )
                res.val( x, y ) = tmp.interp( std::min( T( w / c - 2 ), x / T( c ) ), std::min( T( h / c - 2 ), y / T( c ) ) );

        return res;
    }


    int w, h;
    QVector<T> data;
};

#endif // IMAGE_H
