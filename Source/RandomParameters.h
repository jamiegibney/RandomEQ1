// Declaration of the structure for the random EQ parameter class
// Only low/high shelf and peak filters are available. A shelf filter has a
// random chance (10% by default), but can be modified upon calling Randomise().
// Filter Q is not included.

#pragma once
#include "Filter.h"

using u_int8_t = unsigned char;
using u_int32_t = unsigned int;
using u_int64_t = unsigned long long;

class RandomParameters {
 private:
    u_int32_t mLehmerSeed {};

    u_int32_t Random();

    u_int32_t RandomRange(const u_int32_t&, const u_int32_t&);

    void RandomiseParameters();

    void InitialiseSeed();

    void DetermineType();

    std::vector<float> mGainOptionsDb, mFreqOptionsHz;

    static constexpr u_int8_t mDefaultShelfChance = 10;
    u_int8_t mShelfChance {};

 public:
    float mGain {}, mFreq {};
    FilterType mType;

    RandomParameters();

    RandomParameters(const float&, const float&, const FilterType&);

    bool operator==(const RandomParameters&) const;

    void Randomise(const u_int8_t& shelfChance = mDefaultShelfChance);

    bool useRandomOther = true;

};