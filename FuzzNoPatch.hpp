#ifndef __FuzzNoPatch_hpp__
#define __FuzzNoPatch_hpp__

#include "StompBox.h"

class FuzzNoPatch : public Patch {
public:
    int samplerate = (float)getSampleRate();
    
    //    float *compressorGain;
    
    FuzzNoPatch()
    {
        registerParameter(PARAMETER_A, "Shape");
        registerParameter(PARAMETER_B, "Offset");
        registerParameter(PARAMETER_C, "Gain");
        registerParameter(PARAMETER_D, "Volume");
        registerParameter(PARAMETER_E, "");
    }
    void processAudio(AudioBuffer &buffer)
    {
        float shape = 0.2 * getParameterValue( PARAMETER_A );
        float offset = 1.1 * getParameterValue( PARAMETER_B );
        float gain = 1 + 199.0 * getParameterValue( PARAMETER_C );
        float volume = getParameterValue( PARAMETER_D );
        
        int size = buffer.getSize( );
        for ( int ch = 0; ch < buffer.getChannels( ); ch++ ) {
            float* buf = buffer.getSamples(ch);
            
      
            for ( int i = 0; i < size; i++ ) {
                buf[i] = gain * buf[i];
                if ( buf[i] > 1 ) buf[i] = 1 + shape * buf[i];
                if ( buf[i] < -1 ) buf[i] = -1 + shape * buf[i];
            }
            for ( int i = 0; i < size; i++ )
                buf[i] =( volume * buf[i] * ( 20.0 / gain ) ) + offset;
        }
        
       
    }
    
};

#endif   // __FuzzNoPatch_hpp__

