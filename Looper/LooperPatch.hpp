#ifndef __LooperPatch_hpp__
#define __LooperPatch_hpp__

#include "StompBox.h"
#include "TapTempo.hpp"


#define SAMPLESIZE ( 1 << 17 )

#define TRIGGER_LOW 0.1
//#define TRIGGER_HIGH 0.98
#define TRIGGER_HIGH 0.8

class LooperPatch : public Patch {
private:
    int sampleRate = ( float ) getSampleRate( );

    TapTempo tempo;

    FloatArray loop;
    bool recording;
    int recordingIndex;

    bool playing;
    float playingIndex;

    float playingLength;
    int multiplier;
    float randomizer;
    float speed;
    float blend;
    float trigg;

    bool triggState = false;
    bool reTriggState = false;

public:
    LooperPatch( ) : tempo( 8000 ) {
        registerParameter( PARAMETER_A, "Length" );
        registerParameter( PARAMETER_B, "Randomizer" );
        registerParameter( PARAMETER_C, "Speed" );
        registerParameter( PARAMETER_D, "Blend" );
        registerParameter( PARAMETER_E, "Trigg" );

        loop = FloatArray::create( SAMPLESIZE );
        recording = false;
        recordingIndex = 0;
        playing = false;
        playingIndex = 0;

    }
    ~LooperPatch( ) {
        FloatArray::destroy( loop );
    }

    void processAudio( AudioBuffer &buffer ) {
        int size = buffer.getSize( );

		tempo.trigger( isButtonPressed( PUSHBUTTON ) );

		float length = 4096 * ( 1 - getParameterValue( PARAMETER_A ) );
		tempo.clock( size );
		tempo.setSpeed( length );
		playingLength = tempo.getPeriod( ) * SAMPLESIZE / multiplier;
		randomizer = getParameterValue( PARAMETER_B );
        speed = 0.4 + 1.1 * getParameterValue( PARAMETER_C );
        if ( speed > 0.9 && speed < 1.1 )
            speed = 1;
        else if ( speed <= 0.9 )
            speed = speed + 0.1;
        else if ( speed >= 1.1 )
            speed = speed - 0.1;
        blend = getParameterValue( PARAMETER_D );
        trigg = getParameterValue( PARAMETER_E );


        if ( trigg > TRIGGER_LOW && triggState == false ) {
            triggState = true;
            recording = true;
            playing = true;
            recordingIndex = 0;
            playingIndex = 0;
        }
        if ( trigg <= TRIGGER_LOW ) {
            triggState = false;
            recording = false;
			playing = false;
        }
        if ( trigg > TRIGGER_HIGH && reTriggState == false ) {
            reTriggState = true;
            int power = randomizer < 0.1 ? rand( ) % 3 + 1 : ( int ) ( randomizer * 5 + 1 );
            multiplier = pow( 2, power );
        }
        if ( trigg <= TRIGGER_HIGH ) {
            reTriggState = false;
            multiplier = 1;
        }

        // RECORD LOOP
        if ( recording ) {
            float* buf = buffer.getSamples( 0 );
            float* loo = loop.getData( );
            for ( int i = 0; i < size; i++ ) {
                loo[recordingIndex++] = buf[i];
                if ( recordingIndex >= SAMPLESIZE ) {
                    recording = false;
                    recordingIndex = 0;
                }
                if ( recordingIndex >= playingLength ) {
                    playing = true;
                }
            }
        }

        // PLAY LOOP
        if ( playing ) {
            float* buf = buffer.getSamples( 0 );
            float* loo = loop.getData( );

            float wet = blend;
            float dry = 1 - blend;
            for ( int i = 0; i < size; i++ ) {
                buf[i] = loo[(int)playingIndex] * wet + buf[i] * dry;
                playingIndex += speed;
                if ( playingIndex >= playingLength ) {  // Fin del loop
                    playingIndex = 0;
                }
            }
        }
    }
};
#endif // __LooperPatch_hpp__
