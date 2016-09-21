#ifndef __CajaAzulPatch_hpp__
#define __CajaAzulPatch_hpp__

#include "BistableD.hpp"
#include "ToneBiquad.hpp"
#include "StompBox.h"

#define FUZZ_THRESHOLD 0.002
#define SUBOCT_THRESHOLD 0.002

class CajaAzulPatch : public Patch {
private:
	BistableD d;
	ToneBiquad filter;

public:
	CajaAzulPatch( ) {
		registerParameter( PARAMETER_A, "Fuzz" );
		registerParameter( PARAMETER_B, "SubOct" );
		registerParameter( PARAMETER_C, "Range" );
		registerParameter( PARAMETER_D, "Dry/Wet" );
		registerParameter( PARAMETER_E, "" );

		filter.init();
		filter.setCoeffs( 1, getSampleRate( ) );
		filter.updateStateCoeffs( );
	}
	void processAudio( AudioBuffer &buffer ) {
		float fuzz = getParameterValue( PARAMETER_A );
		float subOct = getParameterValue( PARAMETER_B );
		float blend = getParameterValue( PARAMETER_D );

		float fc = 2 * powf( 10, 3 * getParameterValue( PARAMETER_C ) + 1 /*2.5*/ ) + 20;
		filter.setCoeffs(fc, getSampleRate());



		float* left = buffer.getSamples( 0 );
		int size = buffer.getSize( );


		float filteredSample;
		float fuzzSample;
		float subOctSample;

		for ( int i = 0; i < size; i++ ) {


			if ( left[i] > FUZZ_THRESHOLD ) {
				fuzzSample = 1;
			} else if ( left[i] < -FUZZ_THRESHOLD ) {
				fuzzSample = - 1;
			} else {
				fuzzSample = 0;
			}

			filteredSample = left[i];
			for ( int i = 0; i < 30; i++ )
				filteredSample = filter.processSample( filteredSample, size, i );
			if ( filteredSample > SUBOCT_THRESHOLD ) {
				subOctSample = 1;
			} else if ( filteredSample < -SUBOCT_THRESHOLD ) {
				subOctSample = - 1;
			} else {
				subOctSample = 0;
			}
			subOctSample = d.process( subOctSample != 1 ) ? 1 : -1;

			left[i] = blend * 0.1 * ( fuzz * fuzzSample + subOct * subOctSample ) + ( 1 - blend ) * left[i];

		}
	}
};

#endif // __CajaAzulPatch_hpp__
