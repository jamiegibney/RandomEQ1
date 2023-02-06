// Implementation of the random EQ parameter class, including a Lehmer RNG

#include "RandomParameters.h"

RandomParameters::RandomParameters() {
    InitialiseSeed();

    // TODO user-customisable options would be great:
    mGainOptionsDb = {1.0f, 3.0f, 6.0f, 12.0f};
    mFreqOptionsHz = {125.0f, 250.0f, 500.0f, 1000.0f, 3000.0f, 10000.0f};

    Randomise();
}

RandomParameters::RandomParameters(const float& inGain, const float& inFreq,
                                   const FilterType& inType) : mGain(inGain),
                                   mFreq(inFreq), mType(inType) {}

bool RandomParameters::operator==(const RandomParameters& rhs) const {
    return rhs.mGain == this->mGain && rhs.mFreq == this->mFreq;
}

//                                  //                                      //

// a modified version of a lehmer RNG — quick, high-quality random numbers
u_int32_t RandomParameters::Random() {
    mLehmerSeed += 0xe120fc15;
    u_int64_t tmp;
    tmp = (u_int64_t)mLehmerSeed * 0x4a39b70d;
    u_int32_t m1 = (tmp >> 32) ^ tmp;
    tmp = (u_int64_t)m1 * 0x12fad5c9;
    return (tmp >> 32) ^ tmp;
}

// above, but returns within a range (inclusive) for cleaner expressions
u_int32_t RandomParameters::RandomRange(const u_int32_t& min, const u_int32_t& max) {
    return (Random() % (max - min + 1)) + min;
}

// use the current system time in milliseconds to produce a pseudorandom seed
// needed to set the lehmer seed to something random upon construction
void RandomParameters::InitialiseSeed() {
    mLehmerSeed = std::chrono::time_point_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now()).time_since_epoch().count();
}

void RandomParameters::DetermineType() {
    if(RandomRange(1, 100) < mShelfChance) { // choose shelf
        if(this->mFreq <= 500.0f)
            this->mType = LowShelf;
        else
            this->mType = HighShelf;
    }
    else
        this->mType = Peak;
}

void RandomParameters::Randomise(const u_int8_t& shelfChance) {
    mShelfChance = shelfChance > 100 ? 100 : shelfChance;

    if(useRandomOther) {
        float previousGain = this->mGain,
              previousFreq = this->mFreq;

        while(this->mGain == previousGain && this->mFreq == previousFreq)
            RandomiseParameters();

        return;
    }

    RandomiseParameters();
}

void RandomParameters::RandomiseParameters() {
    float gainPolarity = RandomRange(0, 1) == 1 ? 1.0f : -1.0f;

    this->mGain = mGainOptionsDb[Random() % mGainOptionsDb.size()] * gainPolarity;
    this->mFreq = mFreqOptionsHz[Random() % mFreqOptionsHz.size()];
    DetermineType();
}
