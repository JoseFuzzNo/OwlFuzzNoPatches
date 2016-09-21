#ifndef __BistableD_hpp__
#define __BistableD_hpp__

class BistableD {
private:
	bool inputState;
	bool outputState;
public:
	BistableD( ) {
		inputState = false;
		outputState = false;
	}
	bool process( bool input ) {
		if ( inputState == false && input == true ) {
			outputState = !outputState;
		}
		inputState = input;
		return outputState;
	}
};

#endif
