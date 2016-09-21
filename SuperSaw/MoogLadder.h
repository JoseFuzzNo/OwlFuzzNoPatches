/*
 
 DESCRIPTION:
 Moog Ladder filter, formed by a cascade of four one pole sections, and a "Oberheim style" multiplexer.
 The multiplexer allows to design a LPF or HPF filter (or anything in between ? test at your own risks !)
 
 This implementation follows closely the one proposed by
 Antti Huovilainen and Vesa VaÌlimaÌki
 in the paper:
 NEW APPROACHES TO DIGITAL SUBTRACTIVE SYNTHESIS
 Helsinki University of Technology Laboratory of Acoustics and Audio Signal Processing
 
 With the additon of a Drive parameter to excite a bit more the non linear component.
 
 LICENSE:
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

/* created by the OWL team 2014 */

#ifndef __MoogLadder_h__
#define __MoogLadder_h__

#include "StompBox.h"

enum fType
{
    LPF=0,
    HPF
};

class MoogLadder {
public:
    MoogLadder();
    ~MoogLadder();
    fType type ;
    void setType(fType t) ;
    void setCoeffs(float w0) ; // for LPF, HPF
    void process(int numSamples, float* buffer, float w0, float res, float drive, float masterGain) ;
    float processLadder(float input,float x1, float y1);
    void setMutiplexer();
    float nonLinear(float x); // tanh approx

private:
    float A, B, C, D, E ; // multiplexer coeffs
    float a1, b0, b1 ; // one pole section coeffs
    float pw0; // previous w0
    float pres; // previous res
    float pmasterGain; // previous master Gain
    float in1, out1, out2, out3, out4;
    float comp;
};

#endif // __MoogLadder_h__
