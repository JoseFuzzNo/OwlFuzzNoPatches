#include "Grain.hpp"

float getRandom( ) {
    return ( rand( ) % 1000 ) / 1000.0;
}

Grain::Grain( ) {

}

Grain::Grain( float *buf, int size, unsigned int sampleRate ) {

    delay = new DelayUnit( buf, size, sampleRate );
    phasor.setSampleRate( sampleRate );

    _grainSize = 0;
    _location = 0;
    _density = 0;
    setSpeed( 1 );

    _locationShuffler = 0;
    _densityShuffler = 0;
}

Grain::~Grain( ) {

}


float Grain::pull( ) {
    float phase = phasor.pull( );

    if ( phase == 0 ) {
        _locationShuffler = getRandom( ) * _location;
        if ( _density < getRandom( ) )
            _densityShuffler = 0;
        else
            _densityShuffler = 1;

    }

    if ( _densityShuffler ) {
        float delayPhase = _speed * _grainSize * phase + _locationShuffler; // TODO optimizable guardando esta variable en la clase
        //float delayPhase = 0;
        float sample = delay->getMillisecondsBefore( delayPhase );
        float window = WINDOW[( int ) ( phase * WINDOW_SIZE )];

        return sample * window;
    }
    return 0;
}

DelayUnit* Grain::getData( ) {
    return delay;
}

// SETTERS

void Grain::setGrainSize( float size ) {    // 0.01 - 0.5
    _grainSize = size;

    // Set Phasor frequency
    phasor.setFrequency( 1.0 / size );
}
void Grain::setLocation( float location ) { // 0 - 5000
    _location = location;
}
void Grain::setDensity( float density ) {   //0.01 - 1
    _density = density;
}
void Grain::setSpeed( float speed ) {   // -500 - 1000
    _speed = ( speed * 1000 - 1000 ) * ( -1 );
}
void Grain::setPhase( float phase ) {
    phasor.setPhase( phase );
}
