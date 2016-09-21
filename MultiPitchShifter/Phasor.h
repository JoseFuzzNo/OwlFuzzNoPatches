#ifndef __Phasor_h__
#define __Phasor_h__

/**
 * Clase que implementa un oscilador con forma en diente de sierra.
 */
class SawOscillator {
private:
	float value;
	float increment;
	unsigned int sampleRate;
public:
	Phasor( unsigned int sr ) {
		value = 0;
		sampleRate = sr;
		increment = 440.0 / sampleRate;
	}

	~SawOscillator( ) {
	}

	/**
	 * Se establece la frecuencia del oscilador.
	 * · F = Fs / N
	 * · N = A * Inc
	 * * Inc = F / Fs;
	 *
	 * @param frequency Frecuencia de oscilación.
	 */
	void setFrequency( float frequency ) {
		increment = frequency / sampleRate;
	}

	/**
	 * Se adquiere el siguiente valor.
	 * @return Siguiente valor.
	 */
	float pull( ) {
		value += increment;
		if ( value > 1 ) {
			value = 0;
		}
		if ( value < 0 )
			value = 1;
		return value;
	}

};

#endif // __Phasor_h__
