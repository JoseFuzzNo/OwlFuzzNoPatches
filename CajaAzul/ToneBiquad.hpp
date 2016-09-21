#ifndef __ToneBiquad_hpp__
#define __ToneBiquad_hpp__

class ToneBiquad {
public:
    ToneBiquad() {}
    ~ToneBiquad() {}
    void init(){
        x1 = 0.f;
        x2 = 0.f;
        y1 = 0.f;
        y2 = 0.f;
    }
    void updateStateCoeffs(){
        for (int i=0; i<3; i++) {
            pa[i]=a[i];
            pb[i]=b[i];
        }
    }
    void setCoeffs(float fc, float fs) {
        // Compute the filters coefficients a[i] and b[i]
        float w = 2*M_PI*fc/fs;
        float C = cosf(w);
        float alpha = sin(w)/1.414f; // Q Butterworth

        // LPF 2nd order
        b[0] = (1-C)/2;
        b[1] = 1-C;
        b[2] = b[0];

        a[0] = 1+alpha;
        a[1] = -2*C;
        a[2] = 1-alpha;
    }
    float processSample(float input, int numSamples,int i){
        // process 1 sample at a time, with linear parameter interpolation between start and end of block
        float a1, a2, b0, b1, b2;
        a1 = a[1]/a[0]*i+pa[1]/pa[0]*(numSamples-i);
        a2 = a[2]/a[0]*i+pa[2]/pa[0]*(numSamples-i);
        b0 = b[0]/a[0]*i+pb[0]/pa[0]*(numSamples-i);
        b1 = b[1]/a[0]*i+pb[1]/pa[0]*(numSamples-i);
        b2 = b[2]/a[0]*i+pb[2]/pa[0]*(numSamples-i);
        float output = (b0*input+b1*x1+b2*x2-a1*y1-a2*y2)/numSamples ;
        // store values for biquad state
        x2 = x1;
        x1 = input;
        y2 = y1;
        y1 = output;
        return output;
    }
private:
    float a[3] ; // ai coefficients
    float b[3] ; // bi coefficients
    float pa[3] ; // ai coefficients
    float pb[3] ; // bi coefficients
    float x1, x2, y1, y2 ; // state variables to compute samples
};

#endif // __ToneBiquad_hpp__
