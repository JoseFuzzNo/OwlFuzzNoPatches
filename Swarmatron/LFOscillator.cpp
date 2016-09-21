/*
 * LFOscillator.cpp
 *
 *  Created on: 4 jul. 2016
 *      Author: jgranja
 */

#include "LFOscillator.h"

LFOscillator::LFOscillator( float sr ) {
	sampleRate = sr;
	phaseInc = 440.0 / sampleRate;
	phase = 0;
}

LFOscillator::~LFOscillator() {
}

void LFOscillator::setSampleRate( float sr ) {
	float f = sampleRate * phaseInc;
	sampleRate = sr;
	phaseInc = f / sampleRate;
}

void LFOscillator::setFrequency( float value ) {
	phaseInc = value / sampleRate;
}

float LFOscillator::getNextSample( ) {
	float value = sin( phase * 2 * M_PI ) * 0.5 + 0.5;
	phase += phaseInc;

	if ( phase >= 1 )
		phase -= 1;
	return value;
}

float LFOscillator::peek( ) {
	return sin( phase * 2 * M_PI ) * 0.5 + 0.5;
}

