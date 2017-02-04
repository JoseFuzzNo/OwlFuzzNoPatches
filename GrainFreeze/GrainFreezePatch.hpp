#ifndef __GrainFreezePatch_hpp__
#define __GrainFreezePatch_hpp__

#include "Patch.h"
#include "Grain.hpp"

#define GRAIN_BUFFER_SIZE 196608
#define NUMBER_OF_GRAINS 18
#define SAMPLE_RATE	( uint32_t ) ( 48000 )      /* Hz */

class GrainFreezePatch : public Patch {
private:
    FloatArray grainBuffer;
    Grain grains[NUMBER_OF_GRAINS];
    int bufferIndex;

    SmoothValue<float> grainSize;
    SmoothValue<float> spread;
    float density;
    float blend;
    float speed;
    bool freeze;


public:
    GrainFreezePatch( ) {

        registerParameter( PARAMETER_A, "Grain Size" );
        registerParameter( PARAMETER_B, "Spread" );
        registerParameter( PARAMETER_C, "Density" );
        registerParameter( PARAMETER_D, "Dry/Wet" );
        registerParameter( PARAMETER_E, "" );

        grainBuffer = FloatArray::create( GRAIN_BUFFER_SIZE );
        float *gBuf = grainBuffer.getData( );
        for ( int i = 0; i < NUMBER_OF_GRAINS; i ++ ) {
            grains[i] = Grain( gBuf, GRAIN_BUFFER_SIZE, SAMPLE_RATE );

            // Two good options for starting point
            //grains[i].setPhase( ( rand( ) % 1000 ) / 1000.0 );
            grains[i].setPhase( ( float ) i / NUMBER_OF_GRAINS );
        }

        bufferIndex = 0;

        speed = 1;
        freeze = false;


    }
    ~GrainFreezePatch( ) {

    }

    float speeds[4] = { -1, 0.5, 1, 2 };
    void buttonChanged( PatchButtonId bid, uint16_t value, uint16_t samples ) {
        // Changing the speed doesnt work well
        /*static uint8_t index = 0;
        if ( bid == PUSHBUTTON ) {
            if ( value ) {
                speed = speeds[index];
                if ( ++index >= 4 )
                    index = 0;
            }
        }*/
        if ( !isButtonPressed( BYPASS_BUTTON ) ) {
            if ( bid == PUSHBUTTON ) {
                if ( value ) {
                    freeze = !freeze;
                }
            }
        }
    }

    void processAudio( AudioBuffer &buffer ) {
        // Get and apply parameters
        grainSize = getParameterValue( PARAMETER_A ) * 0.3 + 0.01 + getParameterValue( PARAMETER_E ) * 0.3;
        spread = getParameterValue( PARAMETER_B ) * 4097 + 1;
        density = getParameterValue( PARAMETER_C ) + 0.01;
        blend = getParameterValue( PARAMETER_D );

        if ( isButtonPressed( BYPASS_BUTTON ) )
            freeze = false;
        setButton( RED_BUTTON, freeze );

        for ( int g = 0; g < NUMBER_OF_GRAINS; g++ ) {
            grains[g].setGrainSize( grainSize );
            grains[g].setLocation( spread );
            grains[g].setDensity( density );
            grains[g].setSpeed( speed );
        }


        // Write input to buffer and extract values from the grains
        float *buf = buffer.getSamples( 0 );
        float *gBuf = grainBuffer.getData( );
        for ( int i = 0; i < buffer.getSize( ); i++ ) {
            if ( !freeze ) {
                gBuf[bufferIndex] = buf[i];
                bufferIndex++;
                if ( bufferIndex > GRAIN_BUFFER_SIZE )
                    bufferIndex = 0;
            }

            float sample = 0;
            for ( int g = 0; g < NUMBER_OF_GRAINS; g++ ) {
                grains[g].getData( )->write( buf[i], true );
                sample += grains[g].pull( );

            }
            buf[i] = 0.5 * sample * blend + buf[i] * ( 1 - blend );

        }
    }

};

#endif // __GrainFreezePatch_hpp__


