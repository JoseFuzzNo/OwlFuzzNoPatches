#ifndef __RectifierPatch_hpp_
#define __RectifierPatch_hpp_

#include "StompBox.h"

class RectifierPatch : public Patch {
private:
public:
	RectifierPatch( ) {
		registerParameter( PARAMETER_A, "" );
		registerParameter( PARAMETER_B, "" );
		registerParameter( PARAMETER_C, "" );
		registerParameter( PARAMETER_D, "Blend" );
		registerParameter( PARAMETER_E, "" );
	}

	void processAudio( AudioBuffer &buffer ) {

		float blend = getParameterValue( PARAMETER_D );

		for ( int ch = 0; ch < buffer.getChannels( ); ch++ ) {
			float* buf = buffer.getSamples( ch );
			int size = buffer.getSize( );

			for ( int i = 0; i < size; i++ ) {
				if ( buf[i] < 0 )
					buf[i] = buf[i] * ( 1 - 2 * blend );
			}
		}

	}
};

#endif // __RectifierPatch_hpp_
