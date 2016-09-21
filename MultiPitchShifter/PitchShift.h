#ifndef __PitchShift_h__
#define __PitchShift_h__

#include "DelayUnit.h"
#include "Phasor.h"

class Playback {
private:
	DelayUnit* delay;
	const double pi = 3.14159265358979323846264338328;
public:
	Playback( float* buffer, int size, unsigned int sr ) {
		delay = new DelayUnit( buffer, size, sr );
	}
	Playback( int size, unsigned int sr ) {
		delay = new DelayUnit( size, sr );
	}
	inline float get( float index /* de 0 a 1 */ ) {

		float delayTime = index * 50;
		float pitchedSample = delay->getMillisecondsBefore( delayTime );
		float windowIndex = ( index - 0.5 ) * 0.5;
		float window = cos( 2 * pi * windowIndex );

		return pitchedSample * window;
		//return cos( 2 * pi * ( index - 0.5 ) * 0.5 ) * delay->getMillisecondsBefore( index * 50 );
	}
	DelayUnit* getData( ) {
		return delay;
	}
};

class PitchShift {
private:
	Playback playback;
	SawOscillator phasor;

	float pitch;

public:
	PitchShift( float* buf, int size, unsigned int sampleRate, float pitch ) :
		playback( buf, size, sampleRate ),
		phasor( sampleRate ) {
		setPitch( pitch );
	}
	PitchShift( int size, unsigned int sampleRate, float pitch ) :
		playback( size, sampleRate ),
		phasor( sampleRate ) {
		setPitch( pitch );
	}

	float wrap( float val ) {
		float rest = val - ( int ) val;
		return val >= 0 ? rest : 1 + rest;
	}

	void setPitch( float p ) {
		pitch = p;
		phasor.setFrequency( ( pitch - 1 ) * ( -20 ) );
	}

	inline float get( ) {
		/*float phase = phasor.pull( );
		float residue = wrap( phase + 0.5 );
		return playback.get( phase ) + playback.get( residue );*/
		float phase = phasor.pull( );
		return playback.get( phase ) + playback.get( wrap( phase + 0.5 ) );
	}

	DelayUnit* getData( ) {
		return playback.getData( );
	}

};


#endif // __PitchShift_h__
