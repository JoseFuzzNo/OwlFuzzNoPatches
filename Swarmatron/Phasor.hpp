#ifndef __Phasor_hpp__
#define __Phasor_hpp__

/**
 * Clase que implementa un oscilador con forma en diente de sierra.
 */
class Phasor {
private:
	float value;
	float increment;
	unsigned int sampleRate;
public:

	Phasor( ) {
		value = 0;
		sampleRate = 48000;
		increment = 440.0 / sampleRate;
	}

	/**
	 * Se configura la frecuencia de muestreo.
	 */
	void setSampleRate( unsigned int sr ) {
		sampleRate = sr;
		increment = 440.0 / sampleRate;
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
