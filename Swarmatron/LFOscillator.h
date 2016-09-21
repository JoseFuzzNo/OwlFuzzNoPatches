/*
 * LFOscillator.h
 *
 *  Created on: 4 jul. 2016
 *      Author: jgranja
 */

#ifndef __LFOscillator_h__
#define __LFOscillator_h__

#include "Oscillator.h"
#include "basicmaths.h"

class LFOscillator : public Oscillator {
private:
	float sampleRate;
	float phaseInc;
	float phase;

public:
	LFOscillator( float sampleRate );
	virtual ~LFOscillator( );

	void setSampleRate( float sampleRate );
	void setFrequency( float value );
	float getNextSample( );
	float peek( );
};

#endif /* __LFOscillator_h__ */
