// Declaration of the class structure for a biquad filter
// This model can support more filter types

#pragma once
#include <cmath>
#include <vector>
#include <chrono>

enum FilterType {
    LowShelf = 1,
    HighShelf,
    Peak
};

class Filter {
 private:
    void SetCoefficients();
    // slower (but more accurate) coefficient calculations, mainly applies to shelves
    void SetCoefficientsSlow();

    // precalculate sqrt(2) to improve performance
    const double sqrt2 = sqrt(2);

    FilterType mType {};
    int mSampleRate {};

    // coefficients / processing / parameters
    double a0 {}, a1 {}, a2 {}, b1 {}, b2 {},
           z1 {}, z2 {},
           mFreq {}, mGain {};

    int coefCalculateTime {};

 public:
    Filter();

    void SetSampleRate(const int& sampleRate);

    void SetParameters(const FilterType& type, const double& freq,
                       const double& q, const double& gain);

    double Process(const double&);
    float Process(const float&);

    // returns the time taken to calculate filter coefficients in nanoseconds
    // because why not, it's stupid fast
    int GetCoefficientProcessTime() const;

    // used to bypass the filter processing (saves performance too)
    bool mEnabled = true;

    double mQ {};

    // uses slightly faster shelf coefficient calculations at the cost of precision
    // presumably the compiler optimises most of it anyway, so may be negligible
    bool useFastProcessing = true;
};