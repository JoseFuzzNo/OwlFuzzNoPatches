#ifndef __FreezePatch_hpp__
#define __FreezePatch_hpp__

#include "Patch.h"
#include "Grain.h"

#define GRAIN_COUNT 5
#define GRAIN_BUFFER_SIZE 40000

#define TRIGGER_LOW 0.1
#define TRIGGER_HIGH 0.98


/**
 * Clase para manejar el estado del pedal.
 * @author jose
 */
class State {
public:
	bool		recording;
	bool		soundOnSound;
	bool		playing;
	bool		lastPlay;
	bool		erasing;
	State( ) {
		recording = soundOnSound = playing = lastPlay = erasing = false;
	}
};


/**
 * Clase que maneja la reproducción y manipulación de granos.
 * @author jose
 */
class FreezePatch : public Patch {
private:
	float		grainSize;
	int			grainSizeAdj;
	float		soundOnSoundLevel;
	float		harmonizer;
	float		speed;
	float		blend;
	float		trigger;

	bool		triggState;
	bool		reTriggState;

	float		midiNote;

	State		state;

	/** Buffers **/
	FloatArray	grainBuffer;
	Grain*		grains;
	Grain*		plusOctGrains;
        Grain*          minusOctGrains;

	void play( ) {
		reTriggState = false;
		state.soundOnSound = false;
                state.playing = true;
	}

	void stop( ) {
		triggState = false;
		state.recording = false;
		state.playing = false;
		state.lastPlay = true;
		state.erasing = true;
	}

	void record( ) {
		triggState = true;
		state.erasing = false;
		state.recording = true;
		//state.playing = true;
		state.lastPlay = false;
	}

	void erase( ) {
		triggState = false;
		state.erasing = true;
	}

	void soundOnSound( ) {
		reTriggState = true;
		state.soundOnSound = true;
	}

	float mtof( float midiNote ) {
		return pow( 2, midiNote / 12.0 ) * 8.1757989156 /* Midi C0 */;
	}

	float getParameterCValue( enum PatchParameterId PARAMETER ) {
		float value = getParameterValue( PARAMETER ) * 1.2 - 0.1;
		if ( value > 0.4 && value < 0.6 )
			value = 0.5;
		else if ( value <= 0.4 )
			value += 0.1;
		else if ( value >= 0.6 )
			value -= 0.1;
		return value;
	}

public:
    FreezePatch( ) {
        registerParameter( PARAMETER_A, "S on S" );
        registerParameter( PARAMETER_B, "+1 Oct" );
        registerParameter( PARAMETER_C, "Pitch" );
        registerParameter( PARAMETER_D, "Dry/Wet" );
        registerParameter( PARAMETER_E, "Trigger" );

        grainSize = GRAIN_BUFFER_SIZE;
        grainSizeAdj = 0;
        soundOnSoundLevel = getParameterValue( PARAMETER_A );
        harmonizer = getParameterValue( PARAMETER_B );
        speed = getParameterValue( PARAMETER_C );
        blend = getParameterValue( PARAMETER_D );
        trigger = getParameterValue( PARAMETER_E );
        triggState = false;
        reTriggState = false;

        midiNote = 60;

        grainBuffer = FloatArray::create( GRAIN_BUFFER_SIZE );
        float *gBuf = grainBuffer.getData( );
        
        grains = new Grain( gBuf, GRAIN_BUFFER_SIZE, 1.0 / GRAIN_COUNT );
        Grain* grainPointer = grains;

        for ( int i = 1; i < GRAIN_COUNT; i++ ) {
        	Grain* aux = new Grain( gBuf, GRAIN_BUFFER_SIZE, 1.0 / GRAIN_COUNT );
        	grainPointer->setNext( aux );
        	grainPointer = aux;

        	if ( i == GRAIN_COUNT - 1 )
        		grainPointer->setNext( grains );
        }

        plusOctGrains = new Grain( gBuf, GRAIN_BUFFER_SIZE, 1.0 / GRAIN_COUNT );
        grainPointer = plusOctGrains;

        for ( int i = 1; i < GRAIN_COUNT; i++ ) {
                Grain* aux = new Grain( gBuf, GRAIN_BUFFER_SIZE, 1.0 / GRAIN_COUNT );
                grainPointer->setNext( aux );
                grainPointer = aux;

                if ( i == GRAIN_COUNT - 1 )
                        grainPointer->setNext( plusOctGrains );
        }
        
        
        minusOctGrains = new Grain( gBuf, GRAIN_BUFFER_SIZE, 1.0 / GRAIN_COUNT );
        grainPointer = minusOctGrains;

        for ( int i = 1; i < GRAIN_COUNT; i++ ) {
                Grain* aux = new Grain( gBuf, GRAIN_BUFFER_SIZE, 1.0 / GRAIN_COUNT );
                grainPointer->setNext( aux );
                grainPointer = aux;

                if ( i == GRAIN_COUNT - 1 )
                        grainPointer->setNext( plusOctGrains );
        }
        
        play( );
    }

    ~FreezePatch( ) {
    }

	void buttonChanged( PatchButtonId bid, uint16_t value, uint16_t samples ) {
		if ( bid >= MIDI_NOTE_BUTTON ) {
			if ( value ) { // note on
				midiNote = bid - MIDI_NOTE_BUTTON;
			} else { // note off
			}
		} else if( bid == PUSHBUTTON ) {

		}

    }

    void pollState( ) {
    	if ( grainSizeAdj++ == 750 )
    		grainSizeAdj = 0;
    	grainSize =  GRAIN_BUFFER_SIZE * 0.75 + 5000 * sin( grainSizeAdj / 375.0 * M_PI );
    	soundOnSoundLevel = 0.2 + 0.6 * getParameterValue( PARAMETER_A );
		harmonizer = getParameterValue( PARAMETER_B );

		speed = getParameterCValue( PARAMETER_C) * 24 - 12;
        speed = pow( 2, ( midiNote + speed ) / 12 - 5 );

		blend = getParameterValue( PARAMETER_D );
		trigger = getParameterValue( PARAMETER_E );
		bool button = isButtonPressed( PUSHBUTTON );

		if ( trigger > TRIGGER_LOW && !triggState && !state.recording ) {	// Si hay trigger y no se estaba grabando
			record( );
		}
		if ( trigger <= TRIGGER_LOW && triggState ) {	// Si no hay trigger y se estaba reproduciendo
			erase( );
		}
		if ( trigger > TRIGGER_HIGH && !reTriggState && !state.recording ) {
			soundOnSound( );

		}
		if ( trigger <= TRIGGER_HIGH && !state.soundOnSound ) {
			play( );
		}
		if ( button && state.playing ) {
			soundOnSound( );
		}

    }

    void configureGrain( Grain* grainPointer ) {
		for ( int i = 1; i <= GRAIN_COUNT; i++ ) {
			grainPointer->setSize( grainSize );
			grainPointer->setSpeed( speed );
			grainPointer = grainPointer->next( );
		}
    }

    void processAudio( AudioBuffer &buffer ) {
        pollState( );
        
        configureGrain( grains );
        configureGrain( plusOctGrains );
        configureGrain( minusOctGrains );
        
        // RECORD LOOP
        if ( state.recording ) {
            float* buf = buffer.getSamples( 0 );
            Grain* grainPointer = grains;
            bool reachedEnd = !grainPointer->push( buf, buffer.getSize( ), 1 );
            if ( reachedEnd ) {
                state.recording = false;
                state.playing = true;
            }
        } // ~RECORD LOOP
        
        // ERASE LOOP
        if ( state.erasing ) {
			Grain* grainPointer = grains;
			bool reachedEnd = !grainPointer->erase( buffer.getSize( ) );
			if ( reachedEnd ) {
				state.erasing = false;
				state.playing = true;
			}
        }
        

        // SOUND ON SOUND LOOP
        if ( state.soundOnSound ) {
            float* buf = buffer.getSamples( 0 );
            Grain* grainPointer = grains;
            bool reachedEnd = !grainPointer->push( buf, buffer.getSize( ), soundOnSoundLevel );
            if ( reachedEnd ) {
                //reTriggState = false;
				state.soundOnSound = false;
            }

        } // ~SOUND ON SOUND LOOP

        // PLAY LOOP
        if ( state.playing ) {
            float* bufL = buffer.getSamples( 0 );
            float* bufR = buffer.getSamples( 1 );
            float wet = blend;
            float dry = 1 - blend;

            Grain* grainPointer;
            Grain* plusOctGrainPointer;
            Grain* minusOctGrainPointer;
            float sample;
            float plusOctSample;
            float minusOctSample;
            for ( int i = 0; i < buffer.getSize( ); i++ ) {
                grainPointer = grains;
                plusOctGrainPointer = plusOctGrains;
                minusOctGrainPointer = minusOctGrains;
                sample = 0;
                plusOctSample = 0;
                minusOctSample = 0;
                for ( int j = 0; j < GRAIN_COUNT; j++ ) {
                    sample += grainPointer->pull( );
                    plusOctSample += plusOctGrainPointer->pull( );
                    minusOctSample += minusOctGrainPointer->pull( );
                    
                    grainPointer = grainPointer->next( );
                    plusOctGrainPointer = plusOctGrainPointer->next( );
                    minusOctGrainPointer = minusOctGrainPointer->next( );
                    
                }
                bufL[i] = bufR[i] = wet * ( sample + harmonizer * ( 0.5 * plusOctSample + minusOctSample ) ) + dry* bufL[i];

            }
        } // ~PLAY LOOP

    }
};


#endif // __FreezePatch_hpp__
