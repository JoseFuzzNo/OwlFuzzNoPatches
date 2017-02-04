#ifndef __Grain_hpp__
#define __Grain_hpp__

#include "Window.h"
#include "DelayUnit.hpp"
#include "Phasor.hpp"
#include "basicmaths.h"

class Grain {
private:
    DelayUnit *delay;
    Phasor phasor;

    float _grainSize;
    float _location;
    float _density;
    float _speed;

    float _locationShuffler;
    float _densityShuffler;

public:
    Grain( );
    Grain( float *buf, int size, unsigned int sampleRate );
    ~Grain( );

    float pull( );
    DelayUnit *getData( );

    // SETTERS
    void setGrainSize( float size );
    void setLocation( float location );
    void setDensity( float density );
    void setSpeed( float speed );
    void setPhase( float phase );

};

#endif // __Grain_hpp__
