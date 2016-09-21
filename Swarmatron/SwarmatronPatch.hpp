#ifndef __SwarmatronPatch_hpp__
#define __SwarmatronPatch_hpp__

#include "LFOscillator.h"
#include "MoogLadder.h"
#include "Phasor.hpp"
#include "Patch.h"
#include "Envelope.h"

#define N_SAWS					15

#define SWARM_TUNE				PARAMETER_A
#define SWARM_DETUNE			PARAMETER_B
#define SWARM_NSAWS				PARAMETER_C
#define SWARM_VOLUME			PARAMETER_D
#define SWARM_CUTOFF			PARAMETER_E
#define SWARM_Q					PARAMETER_F
#define SWARM_PITCH_BEND		PARAMETER_G

#define SWARM_VCA_ATTACK		PARAMETER_AA
#define SWARM_VCA_DECAY			PARAMETER_AB
#define SWARM_VCA_SUSTAIN		PARAMETER_AC
#define SWARM_VCA_RELEASE		PARAMETER_AD

#define SWARM_VCF_ATTACK		PARAMETER_AE
#define SWARM_VCF_DECAY			PARAMETER_AF
#define SWARM_VCF_SUSTAIN		PARAMETER_AG
#define SWARM_VCF_RELEASE		PARAMETER_AH
#define SWARM_VCF_ENV			PARAMETER_BA

#define SWARM_LFO_SPEED			PARAMETER_BB
#define SWARM_LFO_VCA_DEPTH		PARAMETER_BC
#define SWARM_LFO_VCF_DEPTH		PARAMETER_BD
#define SWARM_LFO_VCO_DEPTH		PARAMETER_BE

#define SWARM_PORTAMENTO		PARAMETER_BF

class SwarmatronPatch : public Patch {
private:
	Phasor phasors[N_SAWS];

	float frequency;
	float midiNoteModifier;
	float detune;
	float volume;
	float cutoff;
	float q;
	float midiNote;
	int nSaws;
	float lfoVcaDepth;
	float lfoVcfDepth;
	float lfoVcoDepth;
	float portamento;

	AdsrEnvelope vcaEnv;
	MoogLadder ladder;
	AdsrEnvelope vcfEnv;
	LFOscillator lfo;

	float mtof( float midiNote ) {
		return powf( 2, midiNote / 12.0 ) * 8.1757989156 /* Midi C0 */;
	}

	void registerParameters( ) {
		registerParameter( SWARM_TUNE, "Tune" );
		registerParameter( SWARM_DETUNE, "Detune" );
		registerParameter( SWARM_NSAWS, "N of Saws" );
		registerParameter( SWARM_VOLUME, "Volume" );
		registerParameter( SWARM_CUTOFF, "Cutoff" );
		registerParameter( SWARM_Q, "Q" );
		registerParameter( SWARM_PITCH_BEND, "Pitch Bend" );

		registerParameter( SWARM_VCA_ATTACK, "VCA Attack" );
		registerParameter( SWARM_VCA_DECAY, "VCA Decay" );
		registerParameter( SWARM_VCA_SUSTAIN, "VCA Sustain" );
		registerParameter( SWARM_VCA_RELEASE, "VCA Release" );

		registerParameter( SWARM_VCF_ATTACK, "VCF Attack" );
		registerParameter( SWARM_VCF_DECAY, "VCF Decay" );
		registerParameter( SWARM_VCF_SUSTAIN, "VCF Sustain" );
		registerParameter( SWARM_VCF_RELEASE, "VCF Release" );
		registerParameter( SWARM_VCF_ENV, "VCF Env" );

		registerParameter( SWARM_LFO_SPEED, "LFO Speed" );
		registerParameter( SWARM_LFO_VCA_DEPTH, "LFO VCA Depth" );
		registerParameter( SWARM_LFO_VCF_DEPTH, "LFO VCF Depth" );
		registerParameter( SWARM_LFO_VCO_DEPTH, "LFO VCO Depth" );

		registerParameter( SWARM_PORTAMENTO, "Portamento" );

		setParameterValue( SWARM_VCA_SUSTAIN, 1.0 );
		setParameterValue( SWARM_VCF_SUSTAIN, 1.0 );
	}

	void updateParameters( ) {
		detune = getParameterValue( SWARM_DETUNE );

		nSaws = 1 + ( N_SAWS - 1 ) * getParameterValue( SWARM_NSAWS );
		nSaws = nSaws < 0.05 ? 0 : nSaws;

		volume = getParameterValue( SWARM_VOLUME );
		volume = volume < 0.005 ? 0 : volume;

		midiNoteModifier = 0.95 * midiNoteModifier + 0.05 * (
				getParameterValue( SWARM_TUNE ) * 12 +
				getParameterValue( SWARM_PITCH_BEND ) * 12 +
				( lfo.peek( ) - 0.5 ) * lfoVcoDepth * 5
		);

		frequency = portamento * frequency + ( 1 - portamento ) * mtof( midiNote + midiNoteModifier );

		float f = 1 - getParameterValue( SWARM_CUTOFF ) + getParameterValue( SWARM_VCF_ENV ) * vcfEnv.getNextSample( ) + ( lfo.peek( ) - 0.5 ) * lfoVcfDepth;
		cutoff = 2 * powf( 10, 3 * f + 1 ) + 20;
		// Setting the sample rate of the envelope to ( sampleRate / blockSize ) makes the envelope to retrigger on release.
		for ( int i = 0; i < getBlockSize( ); i++ )
			vcfEnv.getNextSample( );

		q = getParameterValue( SWARM_Q ) * 1.1 + 0.1;

		vcaEnv.setAttack( getParameterValue( SWARM_VCA_ATTACK ) );
		vcaEnv.setDecay( getParameterValue( SWARM_VCA_DECAY ) );
		vcaEnv.setSustain( getParameterValue( SWARM_VCA_SUSTAIN ) );
		vcaEnv.setRelease( getParameterValue( SWARM_VCA_RELEASE ) );

		vcfEnv.setAttack( getParameterValue( SWARM_VCF_ATTACK ) );
		vcfEnv.setDecay( getParameterValue( SWARM_VCF_DECAY ) );
		vcfEnv.setSustain( getParameterValue( SWARM_VCF_SUSTAIN ) );
		vcfEnv.setRelease( getParameterValue( SWARM_VCF_RELEASE ) );

		lfo.setFrequency( getParameterValue( SWARM_LFO_SPEED ) * 19.5 + 0.5 );

		lfoVcaDepth = getParameterValue( SWARM_LFO_VCA_DEPTH );
		lfoVcfDepth = getParameterValue( SWARM_LFO_VCF_DEPTH );
		lfoVcoDepth = getParameterValue( SWARM_LFO_VCO_DEPTH );

		portamento = log10( getParameterValue( SWARM_PORTAMENTO ) * 0.9 + 0.1 ) + 1;
	}

	void noteOn( int note ) {
            vcaEnv.gate( true );
            vcfEnv.gate( true );
            midiNote = note;
        }
        
        void noteOff( ) {
            vcaEnv.gate( false );
            vcfEnv.gate( false );
        }
public:
	SwarmatronPatch( ) : vcaEnv( getSampleRate( ) ), vcfEnv( getSampleRate( ) /*/ getBlockSize( )*/ ), lfo( getSampleRate( ) ) {
		registerParameters( );
		for ( int i = 0; i < N_SAWS; i++ ) {
			phasors[i].setSampleRate( getSampleRate( ) );
		}
		updateParameters( );
		ladder.setType( LPF );
		ladder.setMutiplexer( );
		ladder.setCoeffs( 0.f );

		vcaEnv.setSustain( 1.0 );
	}

	void buttonChanged( PatchButtonId bid, uint16_t value, uint16_t samples ) {
		if ( bid >= MIDI_NOTE_BUTTON ) {

			if( value ){ // note on
				noteOn( bid - MIDI_NOTE_BUTTON );
				setButton( PUSHBUTTON, ON, samples + getBlockSize( ) );
			} else { // note off
				if ( bid - MIDI_NOTE_BUTTON == midiNote ) {
					noteOff( );
					setButton( PUSHBUTTON, OFF, samples+getBlockSize( ) );
				}
			}
		} else if ( bid == PUSHBUTTON ) {
                    if ( value ) {
                        noteOn( 36 );
                    } else {
                        noteOff( );
                    }
                }
	}

	void processAudio( AudioBuffer &buffer ) {

		updateParameters( );

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

			left[i] = volume * sample / 2 * vcaEnv.getNextSample( ) * ( 1 - lfo.getNextSample( ) * lfoVcaDepth );
		}

		ladder.process(
				size,
				left,
				2 * M_PI * cutoff / getSampleRate( ),
				q,
				1 /* getDrive( ) */,
				1 /* getMasterGain( ) */
		);


		/*for ( int i = 0; i < size; i++ )	// Right channel is a copy of left channel
			right[i] = left[i];*/
                
                /*for ( int i = 0; i < size; i++ ) {
                    left[i] += right[i];
                }*/
	}
};


#endif
