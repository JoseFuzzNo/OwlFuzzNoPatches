#ifndef __MultiPitchShifterPatch_hpp__
#define __MultiPitchShifterPatch_hpp__

#include "PitchShift.h"
#include "StompBox.h"


#define PITCH_BUFFER_SIZE	( uint64_t )	( 1 << 12 )	/*	samples	*/
#define SAMPLE_RATE	( uint32_t )	( 48000 )	/*	Hz		*/

class MultiPitchShifterPatch : public Patch {
private:
	PitchShift minus1Octave;
	PitchShift variableOctave;
	float minus1Volume;
	float originalVolume;
	float variableVolume;
	float variablePitch;
	float expression;

#define FORWARD 1
#define BACKWARD -1
	int8_t direction;

#define PITCHES 4
	const float pitches[PITCHES] = { 1.0, 1.5, 2, 3.0 };


public:
	MultiPitchShifterPatch( ) : 	minus1Octave( PITCH_BUFFER_SIZE, SAMPLE_RATE, 0.5 ),
									variableOctave( PITCH_BUFFER_SIZE, SAMPLE_RATE, 2 ) {
		registerParameter( PARAMETER_A, "Octave Down" );
		registerParameter( PARAMETER_B, "Original" );
		registerParameter( PARAMETER_C, "Variable" );
		registerParameter( PARAMETER_D, "Where" );
		registerParameter( PARAMETER_E, "Exp" );
		minus1Volume = 0;
		originalVolume = 0;
		variableVolume = 0;
		variablePitch = 0;
		expression = 0;
		direction = FORWARD;
	}
	~MultiPitchShifterPatch( ) {
	}

	void processAudio( AudioBuffer &buffer ) {
		minus1Volume = getParameterValue( PARAMETER_A );
		originalVolume = getParameterValue( PARAMETER_B );
		variableVolume = getParameterValue( PARAMETER_C );
		variablePitch = 0.9 * variablePitch + 0.1 * getParameterValue( PARAMETER_D ) * PITCHES;
		expression = getParameterValue( PARAMETER_E );

		if( isButtonPressed( PUSHBUTTON ) ) {
			direction = direction == FORWARD ? BACKWARD : FORWARD;
		}

		variableOctave.setPitch( pitches[( int ) variablePitch] + expression * direction );

		float* buf = buffer.getSamples( 0 );
		for ( int i = 0; i < buffer.getSize( ); i++ ) {
			minus1Octave.getData( )->write( buf[i] );
			variableOctave.getData( )->write( buf[i] );
			buf[i] = minus1Volume * minus1Octave.get( )
					+ originalVolume * buf[i]
				   + variableVolume * variableOctave.get( );
		}

	}
};


#endif // __MultiPitchShifterPatch_hpp__
