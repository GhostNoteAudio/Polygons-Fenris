#pragma once
#include "blocks/Biquad.h"

using namespace Polygons;

namespace Fenris
{
    class Eq
    {
        int samplerate;
        Biquad lowShelf;
        Biquad highShelf;
        Biquad midA;
        Biquad midB;
    public:
        inline Eq(int samplerate) : 
            lowShelf(Biquad::FilterType::LowShelf, samplerate),
            highShelf(Biquad::FilterType::HighShelf, samplerate),
            midA(Biquad::FilterType::Peak, samplerate),
            midB(Biquad::FilterType::Peak, samplerate)                 
        {
            this->samplerate = samplerate;
            lowShelf.Frequency = 100;
            highShelf.Frequency = 4000;
            midA.Frequency = 400;
            midB.Frequency = 1000;
            lowShelf.SetGainDb(0);
            highShelf.SetGainDb(0);
            midA.SetGainDb(0);
            midB.SetGainDb(0);
            midA.SetQ(0.4);
            midB.SetQ(0.4);

            UpdateAll();
        }

        inline void SetParameter(int paramId, double value)
        {
            if (paramId == Parameter::EqLowF)
                lowShelf.Frequency = value;
            else if (paramId == Parameter::EqHighF)
                highShelf.Frequency = value;
            else if (paramId == Parameter::EqMid1F)
                midA.Frequency = value;
            else if (paramId == Parameter::EqMid2F)
                midB.Frequency = value;
            else if (paramId == Parameter::EqLowG)
                lowShelf.SetGainDb(value);
            else if (paramId == Parameter::EqHighG)
                highShelf.SetGainDb(value);
            else if (paramId == Parameter::EqMid1G)
                midA.SetGainDb(value);
            else if (paramId == Parameter::EqMid2G)
                midB.SetGainDb(value);

            UpdateAll();
        }

        inline void UpdateAll()
        {
            lowShelf.Update();
            highShelf.Update();
            midA.Update();
            midB.Update();
        }

        inline void Process(float* input, float* output, int bufSize)
        {
            auto bufHandle = Buffers::Request();
            auto buf = bufHandle.Ptr;
            lowShelf.Process(input, buf, bufSize);
            midA.Process(buf, buf, bufSize);
            midB.Process(buf, buf, bufSize);
            highShelf.Process(buf, output, bufSize);
        }
    };
}