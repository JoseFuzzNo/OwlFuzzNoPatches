#ifndef __DelayUnit_hpp__
#define __DelayUnit_hpp__

#include "FloatArray.h"

/**
 * Esta clase implementa una linea de delay. Es necesario que
 * el tamaño del buffer sea una potencia de 2.
 */
class DelayUnit {
private:
    FloatArray buffer;
    unsigned int writeIndex;
    unsigned int sampleRate;
public:
    /**
     * Constructor por defecto. El buffer queda sin inicializar.
     */
    DelayUnit( ) :
        writeIndex( 0 ),
        sampleRate( 48000 ) {
    }

    /**
     * Constructor con creación de buffer.
     */
    DelayUnit( int size, unsigned int sr ) :
        writeIndex( 0 ),
        sampleRate( sr ) {
        buffer = FloatArray::create( size );
    }

    /**
     * Constructor con asignación de buffer. El delay puede crear
     * su propio buffer pero es posible necesitar utilizar un buffer
     * ya creado por otra instancia del delay.
     */
    DelayUnit( float* buf, int size, unsigned int sr ) :
        buffer( buf, size ),
        writeIndex( 0 ),
        sampleRate( sr ) {
    }

    /**
     * Extrae un valor interpolado del buffer.
     * @param index Milisegundos de distancia al cabezal de escritura.
     */
    inline float getMillisecondsBefore( float milliseconds ) {
        float delay = sampleRate * milliseconds / 1000.0;
        int delayInt = ( int )delay;
        float diff = delay - delayInt;
        return
                diff *
                buffer[writeIndex - delayInt & ( buffer.getSize( ) - 1 )]
                       +
                ( 1 - diff ) *
                buffer[writeIndex - delayInt + 1 & ( buffer.getSize( ) - 1 )];
    }

    /**
     * Devuelve el buffer interno
     */
    float* getData( ) {
        return buffer.getData( );
    }

    /**
     * Mueve el cabezal una posicion adelante y escribe una muestra.
     */
    inline void write( float value, float onlyForwardHead ) {
        if( ++writeIndex == buffer.getSize( ) )
            writeIndex = 0;
        if ( !onlyForwardHead )
            buffer[writeIndex] = value;
    }

    /**
     * Metodo estático para la creación de un buffer.
     */
    static DelayUnit* create( int samples ){
        DelayUnit* delay = new DelayUnit( );
        delay->buffer = FloatArray::create( samples );
        return delay;
    }

    /**
     * Método estático para la destrucción de u buffer.
     */
    static void destroy( DelayUnit* delay ){
        FloatArray::destroy(delay->buffer);
    }
};


#endif // __Delay_hpp__
