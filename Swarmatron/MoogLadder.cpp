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

#include "MoogLadder.h"

MoogLadder::MoogLadder(){
    in1 = 0.f;
    out1 = 0.f;
    out2 = 0.f;
    out3 = 0.f;
    out4 = 0.f;
    A = 0.f;
    B = 0;
    C = 0;
    D = 0;
    E = 0;
    pres=0;
    pw0=0.f;
    pmasterGain=0.f;
    type = LPF;
}

MoogLadder::~MoogLadder(){
}


void MoogLadder::setType(fType t){
    type = t;
}


void MoogLadder::setMutiplexer(){
    switch (type)
    {
        case HPF:
            A=1.0;
            B=-4.0;
            C=6.0;
            D=-4.0;
            E=1.0;
            comp=0.0;
            break;
        case LPF:
            A=0.0;
            B=0.0;
            C=0.0;
            D=0.0;
            E=1.0;
            comp=0.5;
            break;
        default:
            A=0.0;
            B=0.0;
            C=0.0;
            D=0.0;
            E=1.0;
            comp=0.5;
            break;
    }
}

void MoogLadder::setCoeffs(float w0){
    // w0 = 2pi * f/fc
    float g=1-expf(-w0);
    a1=1-g;
    b0=g/1.3;
    b1=g*0.23076923076;
}

float MoogLadder::processLadder(float input, float x1, float y1){
    float output = a1*y1 + b0*input + b1*x1;
    return output;
}

float MoogLadder::nonLinear(float x){
    // tanh approx
    if (x<-3)
        return -1;
    else if (x>3)
        return 1;
    else
        return x * ( 27 + x*x ) / ( 27 + 9*x*x );
}

void MoogLadder::process(int numSamples, float *buffer, float w0, float res, float drive, float masterGain){
    int N = numSamples-1;
    float w0i;
    float resi;
    float masterGaini;
    float a,b,c,d,e,output;

    for (int i=0;i<numSamples;i++){
        // compute perSample w0, res, gain
        if (N>0){
            w0i = (pw0*(N-i)+i*w0)/N;
            resi =(pres*(N-i)+i*res)/N;
            masterGaini=(pmasterGain*(N-i)+i*masterGain)/N;
        }
        else {
            w0i = w0;
            resi=res;
        }

        // update coeffs per sample
        setCoeffs(w0i);

        // compute sections
        float in = nonLinear(buffer[i]* drive);
        a=in-4*resi*(nonLinear(out4)-comp*in);
        b=processLadder(a, in1, out1);
        c=processLadder(b, out1, out2);
        d=processLadder(c,out2,out3);
        e=processLadder(d,out3,out4);

        // Multiplexer
        output = A*a + B*b + C*c + D*d + E*e ;
        buffer[i] = masterGaini * output / powf(drive,0.3f);
        // state variables update
        in1=a;
        out1=b;
        out2=c;
        out3=d;
        out4=e;

    }
    // state variables update 2
    pw0=w0;
    pres=res;
    pmasterGain=masterGain;

}
