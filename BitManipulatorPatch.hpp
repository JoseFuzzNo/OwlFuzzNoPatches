#ifndef __BitManipulatorPatch_hpp__
#define __BitManipulatorPatch_hpp__

#include "StompBox.h"

class BitManipulatorPatch : public Patch {
private:
	int sampleRateCounter;
	int sampleRate;
	float lastSample;
	int bitDepth;
	int sampleKillCounter;
	int sampleKill;
	bool kill;
	float blend;
public:
	BitManipulatorPatch( ) {
		registerParameter(PARAMETER_A, "Bit Depth");
		registerParameter(PARAMETER_B, "Sample Rate");
		registerParameter(PARAMETER_C, "Sample Kill");
		registerParameter(PARAMETER_D, "Dry/Wet");
		registerParameter(PARAMETER_E, "");

		sampleRateCounter = 0;
		sampleRate = 0;
		lastSample = 0;
		bitDepth = 0;
		sampleKillCounter = 0;
		sampleKill = 0;
		kill = false;
		blend = 0;
	}

	void processAudio( AudioBuffer &buffer ) {

		bitDepth = 24 * getParameterValue( PARAMETER_A );
		sampleRate = 50 * getParameterValue( PARAMETER_B ) +
				50 * getParameterValue( PARAMETER_E );
		sampleKill = 50 * getParameterValue( PARAMETER_C ) +
				50 * getParameterValue( PARAMETER_E );
		blend = getParameterValue( PARAMETER_D );

		for( int ch = 0; ch < buffer.getChannels( ); ch++ ) {
			float* buf = buffer.getSamples( ch );
			int size = buffer.getSize( );

			for ( int i = 0; i < size; i++ ) {

				float original = buf[i];
				float newSample = 0;

				/* Bit Crusher */
				float mask = powf( 2, bitDepth );
				long crushedSample = buf[i] * mask;
				newSample = crushedSample / mask;


				/* Sample Rate */
				if ( sampleRateCounter > sampleRate ) {
					sampleRateCounter = 0;
					lastSample = newSample;
				} else {
					newSample = lastSample;
				}
				sampleRateCounter++;


				/* Sample Kill */
				if ( sampleKillCounter > sampleKill ) {
					sampleKillCounter = 0;
					kill = !kill;
				} else if ( kill ){
					newSample = 0;
				}
				sampleKillCounter++;

				buf[i] = blend * newSample + ( 1 - blend ) * buf[i];

			}
		}

	}
};

#endif // __BitManipulatorPatch_hpp__
