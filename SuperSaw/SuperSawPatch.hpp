#ifndef __SuperSawPatch_hpp__
#define __SuperSawPatch_hpp__

#include "Phasor.hpp"
#include "StompBox.h"
#include "MoogLadder.h"

#define N_SAWS 15

class SuperSawPatch : public Patch {
private:
	Phasor phasors[N_SAWS];

	float frequency;
	float detune;
	float volume;

	int nSaws;

	bool roundFrequency;

	MoogLadder ladder; // Moog filter

	float getCutoffFrequency( ) {
		float f = 1 - getParameterValue( PARAMETER_E );
		return 2 * powf( 10, 3 * f + 1 ) + 20;
	}
	float getQ( ){
	    float q = getParameterValue( PARAMETER_F );
	    // param_B = 0    <-> Q=0.5
	    // param_B = 1    <-> Q=1.1
	    return q*1.1+0.1;
	  }
	float mtof( float midiNote ) {
		return pow( 2, midiNote / 12.0 ) * 8.1757989156 /* Midi C0 */;
	}

public:




	SuperSawPatch( ) {
		registerParameter( PARAMETER_A, "Frequency" );
		registerParameter( PARAMETER_B, "Detune" );
		registerParameter( PARAMETER_C, "N of Saws" );
		registerParameter( PARAMETER_D, "Volume" );
		registerParameter( PARAMETER_E, "Cutoff" );
		registerParameter( PARAMETER_F, "Q" );
		for ( int i = 0; i < N_SAWS; i++ ) {
			phasors[i].setSampleRate( ( unsigned int ) getSampleRate( ) );
		}

		frequency = 0;
		detune = 0;
		volume = 0;

		nSaws = 0;

		roundFrequency = false;

		ladder.setType( LPF );
		ladder.setMutiplexer( );
		ladder.setCoeffs( 0.f );
	}



	void processAudio( AudioBuffer &buffer ) {

		//frequency = 5 + 1000 * getParameterValue( PARAMETER_A );
		detune = getParameterValue( PARAMETER_B );
		nSaws = 1 + ( N_SAWS - 1 ) * getParameterValue( PARAMETER_C );
		volume = getParameterValue( PARAMETER_D );
		if ( volume < 0.005 )
			volume = 0;
		if ( isButtonPressed( PUSHBUTTON ) )
			roundFrequency = !roundFrequency;
		float midiNote = getParameterValue( PARAMETER_A ) * 127;
		if ( roundFrequency ) {
			frequency = mtof( floor( midiNote ) );
		} else {
			frequency = 0.95 * frequency + 0.05 * mtof( midiNote );
		}
		//debugMessage( "f:", getParameterValue( PARAMETER_A ) * 127  );

		int centerSaw = nSaws / 2;
		for ( int i = 0; i < nSaws; i++ )
			phasors[i].setFrequency( frequency + frequency * detune * ( i - centerSaw ) / nSaws );


		int size = buffer.getSize( );

		float* left = buffer.getSamples( 0 );
		float* right = buffer.getSamples( 1 );

		for ( int i = 0; i < size; i++ ) {
			float sample = 0;
			for ( int j = 0; j < nSaws; j++ ) {
				sample += phasors[j].pull( ) * 2 - 1;
			}

			left[i] = volume * sample / 2;
		}

		ladder.process(
				size,
				left,
				2 * M_PI * getCutoffFrequency( ) / getSampleRate( ),
				getQ( ),
				1 /*getDrive( )*/,
				1 /*getMasterGain( )*/
		);
		for ( int i = 0; i < size; i++ )
			right[i] = left[i];
	}
};


#endif
