// Implementation of a simple biquad filter, with algorithms for low/high shelf
// and peak filters

#include "Filter.h"
#include "FastSqrt.h"

Filter::Filter() {
    // initialise values
    a0 = 1.0;
    a1 = a2 = b1 = b2 = z1 = z2 = 0.0;
    mType = Peak;
    mFreq = 250.0;
    mQ = 0.707;
    mGain = 0.0;
}

void Filter::SetSampleRate(const int& sampleRate) {
    this->mSampleRate = sampleRate;
}

void Filter::SetParameters(const FilterType& type, const double& freq,
                           const double& q, const double& gain) {
    if(!mEnabled)
        return;

    this->mType = type;
    this->mFreq = freq;
    this->mQ = q;
    this->mGain = gain;

    SetCoefficients();
}

int Filter::GetCoefficientProcessTime() const {
    return coefCalculateTime;
}

void Filter::SetCoefficients() {
    if(!useFastProcessing) {
        SetCoefficientsSlow();
        return;
    }

    auto tStart = std::chrono::high_resolution_clock::now();

    double norm = 0.0,
           v = pow(10, abs(mGain) * 0.05),
           k = tan(M_PI * (mFreq / mSampleRate)),
           k2 = k * k,
           sqrt2V_K = 0.0, sqrt2_K = 0.0;

    // small optimisation to skip processing time for peak filter
    if(this->mType != Peak) {
        sqrt2V_K = (useFastProcessing ? FastSqrt::FS1(2.0 * v) : sqrt(2.0 * v)) * k;
        sqrt2_K = sqrt2 * k;
    }

    // NOTE 06/02 — shelves are disabled for RandomEQ
    switch(this->mType) {
        case LowShelf:
            if(mGain >= 0.0) {
                norm = 1 / (1 + sqrt2_K + k2);
                a0 = (1 + sqrt2V_K + v * k2) * norm;
                a1 = 2 * (v * k2 - 1) * norm;
                a2 = (1 - sqrt2V_K + v * k2) * norm;
                b1 = 2 * (k2 - 1) * norm;
                b2 = (1 - sqrt2_K + k2) * norm;
            }
            else {
                norm = 1 / (1 + sqrt2V_K + v * k2);
                a0 = (1 + sqrt2_K + k2) * norm;
                a1 = 2 * (k2 - 1) * norm;
                a2 = (1 - sqrt2_K + k2) * norm;
                b1 = 2 * (v * k2 - 1) * norm;
                b2 = (1 - sqrt2V_K + v * k2) * norm;
            }
            break;

        case HighShelf:
            if(mGain >= 0.0) {
                norm = 1 / (1 + sqrt2_K + k2);
                a0 = (v + sqrt2V_K + k2) * norm;
                a1 = 2 * (k2 - v) * norm;
                a2 = (v - sqrt2V_K + k2) * norm;
                b1 = 2 * (k2 - 1) * norm;
                b2 = (1 - sqrt2_K + k2) * norm;
            }
            else {
                norm = 1 / (v + sqrt2V_K + k2);
                a0 = (1 + sqrt2_K + k2) * norm;
                a1 = 2 * (k2 - 1) * norm;
                a2 = (1 - sqrt2_K + k2) * norm;
                b1 = 2 * (k2 - v) * norm;
                b2 = (v - sqrt2V_K + k2) * norm;
            }
            break;

        case Peak:
            if(mGain >= 0.0) {
                norm = 1 / (1 + 1 / mQ * k + k2);
                a0 = (1 + v / mQ * k + k2) * norm;
                a1 = 2 * (k2 - 1) * norm;
                a2 = (1 - v / mQ * k + k2) * norm;
                b1 = a1;
                b2 = (1 - 1 / mQ * k + k2) * norm;
            }
            else {
                norm = 1 / (1 + v / mQ * k + k2);
                a0 = (1 + 1 / mQ * k + k2) * norm;
                a1 = 2 * (k2 - 1) * norm;
                a2 = (1 - 1 / mQ * k + k2) * norm;
                b1 = a1;
                b2 = (1 - v / mQ * k + k2) * norm;
            }
            break;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    this->coefCalculateTime = std::chrono::duration<int, std::nano>(tEnd - tStart).count();
}

void Filter::SetCoefficientsSlow() {
    auto tStart = std::chrono::high_resolution_clock::now();

    double norm = 0.0,
           v = pow(10, abs(mGain) / 20),
           k = tan(M_PI * (mFreq / mSampleRate));

    // NOTE despite being more precise, this can still be optimised without losing
    //  precision (repeated sqrt). unsure of how much the compiler optimises, though
    switch(this->mType) {
        case LowShelf:
            if(mGain >= 0.0) {
                norm = 1 / (1 + sqrt(2) * k + k * k);
                a0 = (1 + sqrt(2.0 * v) + v * k * k) * norm;
                a1 = 2 * (v * k * k - 1) * norm;
                a2 = (1 - sqrt(2.0 * v) + v * k * k) * norm;
                b1 = 2 * (k * k - 1) * norm;
                b2 = (1 - sqrt(2) * k + k * k) * norm;
            }
            else {
                norm = 1 / (1 + sqrt(2.0 * v) + v * k * k);
                a0 = (1 + sqrt(2) * k + k * k) * norm;
                a1 = 2 * (k * k - 1) * norm;
                a2 = (1 - sqrt(2) * k + k * k) * norm;
                b1 = 2 * (v * k * k - 1) * norm;
                b2 = (1 - sqrt(2.0 * v) + v * k * k) * norm;
            }
            break;

        case HighShelf:
            if(mGain >= 0.0) {
                norm = 1 / (1 + sqrt(2) * k + k * k);
                a0 = (v + sqrt(2.0 * v) + k * k) * norm;
                a1 = 2 * (k * k - v) * norm;
                a2 = (v - sqrt(2.0 * v) + k * k) * norm;
                b1 = 2 * (k * k - 1) * norm;
                b2 = (1 - sqrt(2) * k + k * k) * norm;
            }
            else {
                norm = 1 / (v + sqrt(2.0 * v) + k * k);
                a0 = (1 + sqrt(2) * k + k * k) * norm;
                a1 = 2 * (k * k - 1) * norm;
                a2 = (1 - sqrt(2) * k + k * k) * norm;
                b1 = 2 * (k * k - v) * norm;
                b2 = (v - sqrt(2.0 * v) + k * k) * norm;
            }
            break;

        case Peak:
            if(mGain >= 0.0) {
                norm = 1 / (1 + 1 / mQ * k + k * k);
                a0 = (1 + v / mQ * k + k * k) * norm;
                a1 = 2 * (k * k - 1) * norm;
                a2 = (1 - v / mQ * k + k * k) * norm;
                b1 = a1;
                b2 = (1 - 1 / mQ * k + k * k) * norm;
            }
            else {
                norm = 1 / (1 + v / mQ * k + k * k);
                a0 = (1 + 1 / mQ * k + k * k) * norm;
                a1 = 2 * (k * k - 1) * norm;
                a2 = (1 - 1 / mQ * k + k * k) * norm;
                b1 = a1;
                b2 = (1 - v / mQ * k + k * k) * norm;
            }
            break;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    this->coefCalculateTime = std::chrono::duration<int, std::nano>(tEnd - tStart).count();
}

double Filter::Process(const double& in) {
    if(!mEnabled)
        return in;

    double out = in * a0 + z1;
    z1 = in * a1 + z2 - b1 * out;
    z2 = in * a2 - b2 * out;
    return out;
}

float Filter::Process(const float& in) {
    if(!mEnabled)
        return in;

    float out = in * a0 + z1;
    z1 = in * a1 + z2 - b1 * out;
    z2 = in * a2 - b2 * out;
    return out;
}