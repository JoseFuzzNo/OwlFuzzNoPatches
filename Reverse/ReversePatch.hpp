#ifndef __ReversePatch_hpp__
#define __ReversePatch_hpp__

#include "StompBox.h"

#define BUFFER_SIZE 50000

class ReversePatch : public Patch {
private:
    FloatArray delayBuffer;
    FloatArray auxBuffer;
    
    int bufferSize;
    float feedback;
    float blend;
    
    int bufferPointer;
    int auxPointer;
    int dumpPointer;
    
    bool dump;
    
public:

    ReversePatch( ) {
      registerParameter(PARAMETER_A, "Delay");
      registerParameter(PARAMETER_B, "Feedback");
      registerParameter(PARAMETER_C, "");
      registerParameter(PARAMETER_D, "Dry/Wet");
      registerParameter(PARAMETER_E, "");

      delayBuffer = FloatArray::create( BUFFER_SIZE );
      auxBuffer = FloatArray::create( BUFFER_SIZE );
      
      bufferSize = getParameterValue( PARAMETER_A ) * BUFFER_SIZE;
      feedback = getParameterValue( PARAMETER_B );
      blend = getParameterValue( PARAMETER_D );
      
      bufferPointer = 0;
      auxPointer = 0;
      dumpPointer = 0;
      
      dump = false;
      
    }
    
  void processAudio( AudioBuffer &buffer ) {
        bufferSize = getParameterValue( PARAMETER_A ) * BUFFER_SIZE;
        
      int size = buffer.getSize( );
      feedback = getParameterValue( PARAMETER_B );
      blend = getParameterValue( PARAMETER_D );
      
      float *buf = buffer.getSamples( 0 );
      float *delBuf = delayBuffer.getData( );
      float *auxBuf = auxBuffer.getData( );
      
      for ( int i = 0; i < size; i++ ) {
          auxBuf[auxPointer] = buf[i] + feedback * auxBuf[auxPointer];
          /*auxPointer++;
          if ( auxPointer >= bufferSize ) {
              auxPointer = 0;
              dump = true;
          }*/
          auxPointer--;
          if ( auxPointer < 0 ) {
              auxPointer = bufferSize - 1;
              dump = true;
          }
          
      }
      
      if ( dump ) {
          /*for ( int i = 0; i < size; i++ ) {
              delBuf[dumpPointer] = auxBuf[dumpPointer];
              dumpPointer++;
              if ( dumpPointer >= bufferSize ) {
                  dumpPointer = 0;
                  dump = false;
              }
          }*/
          
          for ( int i = 0; i < bufferSize; i++ ) {
            delBuf[i] = auxBuf[i];
          }
          dump = false;
      }
      
      
      for ( int i = 0; i < size; i++ ) {
        buf[i] = blend * delBuf[bufferPointer] + ( 1 - blend ) * buf[i];
        bufferPointer++;
        if ( bufferPointer >= bufferSize ) {
            bufferPointer = 0;
        }
      }
      
  }
    
};

#endif   // __ReversePatch_hpp__
