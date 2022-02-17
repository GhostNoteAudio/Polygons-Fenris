#pragma once

#include "Polygons.h"
#include "blocks/ModulatedDelay.h"
#include "blocks/ModulatedAllpass.h"
#include "Constants.h"
#include "blocks/Biquad.h"
#include "blocks/Bitcrusher.h"

using namespace Polygons;

namespace Fenris
{
    class ZzzzRev
    {
        const static int PRE_DIFFUSE_COUNT = 6;
        const static int ZCOUNT = 4;

        ModulatedDelay<FS_MAX/16, BUFFER_SIZE> Delay[ZCOUNT];
        ModulatedAllpass<FS_MAX/20, BUFFER_SIZE> PreDiffuser[PRE_DIFFUSE_COUNT];
        ModulatedAllpass<FS_MAX/20, BUFFER_SIZE> Diffuser[ZCOUNT*2];
        Biquad lpPre, lpPost;

        // Delay lengths in milliseconds, handpicked arbitrarily :)
        float PreDiffuserSizes[PRE_DIFFUSE_COUNT] = {56.797, 65.1785, 69.7954, 75.6531, 83.157, 96.194};
        float DiffuserSizes[ZCOUNT] = {70.312, 78.5123, 87.9312, 92.1576};
        float DelaySizes[ZCOUNT] = {73.459, 95.961, 104.1248, 117.934};

        // Modulation rates in Hz, I used sequential prime numbers scaled down
        float PreDiffuserModRate[PRE_DIFFUSE_COUNT] = {13*0.05, 17*0.05, 19*0.05, 23*0.05, 29*0.05};
        float DiffuserModRate[ZCOUNT] = {31*0.02, 37*0.02, 41*0.02, 43*0.02};
        float DelayModRate[ZCOUNT]    = {47*0.01, 53*0.01, 59*0.01, 61*0.01};

        int Samplerate;
        float Krt;
        float T60;
        float Wet;
        float Dry;
        float EarlySize;
        float DiffuserSize;
        float LateSize;
        float Modulation;
        float DiffuseFeedback;
        int Bits;
    public:
        ZzzzRev(int samplerate) : lpPre(Biquad::FilterType::LowPass, samplerate), lpPost(Biquad::FilterType::LowPass6db, samplerate)
        {
            Samplerate = samplerate;
            Krt = 0.0;
            T60 = 5.0;
            Wet = 0.5;
            Dry = 1.0;
            EarlySize = 0.1;
            DiffuserSize = 1.0;
            LateSize = 0.1;
            Modulation = 0.2;
            DiffuseFeedback = 0.7;
            Bits = 5;
            lpPre.Frequency = 20000;
            lpPost.Frequency = 16000;

            UpdateAll();
        }

        void SetParameters(int paramId, double value)
        {
            if (paramId == Parameter::ReverbDecay)
            {
                if (value < 0.1)
                    value = 0.1;
                T60 = value;
            }
            else if (paramId == Parameter::ReverbDiffuse)
            {
                DiffuseFeedback = 0.5 + (1 - value) * 0.49;
            }
            else if (paramId == Parameter::ReverbBits)
            {
                Bits = (int)value;
            }
            else if (paramId == Parameter::ReverbMix)
            {
                Wet = ClipF(value * 2, 0.0, 1.0);
                Dry = ClipF(2 - value * 2, 0, 1);
            }
            else if (paramId == Parameter::ReverbSizeEarly)
            {
                EarlySize = value; // 10-100%
            }
            else if (paramId == Parameter::ReverbSizeLate)
            {
                LateSize = value; // 10-100%
                DiffuserSize = 0.2 + value * 0.8;
            }
            else if (paramId == Parameter::ReverbModulate)
            {
                Modulation = value;
            }
            else if (paramId == Parameter::ReverbTone)
            {
                auto val2 =  Response4Oct(value);
                lpPre.Frequency = 200 + val2 * 15800;
                lpPost.Frequency = 200 + val2 * val2 * 15800;
            }

            UpdateAll();
        }

        void UpdateAll()
        {
            lpPre.Update();
            lpPost.Update();

            const float IdealisedTimeConstant = 0.15 * std::sqrt(LateSize); // assumed tank round trip time
            auto tcToT60 = T60 / IdealisedTimeConstant;
            auto dbPerTc = -60 / tcToT60;
            Krt = std::pow(10, dbPerTc/20);

            for (size_t i = 0; i < PRE_DIFFUSE_COUNT; i++)
            {
                PreDiffuser[i].Feedback = 0.73;
                PreDiffuser[i].InterpolationEnabled = true;
                PreDiffuser[i].SampleDelay = (int)(PreDiffuserSizes[i] * 0.001 * EarlySize * Samplerate);
                PreDiffuser[i].ModRate = PreDiffuserModRate[i] / Samplerate;
                PreDiffuser[i].ModAmount = Modulation * 25;
            }

            for (size_t i = 0; i < ZCOUNT; i++)
            {
                Diffuser[i].Feedback = DiffuseFeedback;
                Diffuser[i].InterpolationEnabled = true;
                Diffuser[i].SampleDelay = (int)(DiffuserSizes[i] * 0.001 * DiffuserSize * Samplerate);
                Diffuser[i].ModRate = DiffuserModRate[i] / Samplerate;
                Diffuser[i].ModAmount = Modulation * 25;

                Delay[i].SampleDelay = (int)(DelaySizes[i] * 0.001 * LateSize * Samplerate);
                Delay[i].ModRate = DelayModRate[i] / Samplerate;
                Delay[i].ModAmount = Modulation * (i == 0 ? 200 : 25); // extra mod on the first delay
            }
        }

        void Process(float** inputs, float** outputs, int bufSize)
        {
            auto tb = Buffers::Request();
            auto buf = tb.Ptr;

            Copy(buf, inputs[0], bufSize);
            Mix(buf, inputs[1], 1.0, bufSize);
            Bitcrusher::Process(buf, buf, Bits, bufSize);
            lpPre.Process(buf, buf, bufSize);

            float* preDiffIO = buf;
            for (size_t i = 0; i < PRE_DIFFUSE_COUNT; i++)
            {
                PreDiffuser[i].Process(preDiffIO, bufSize);
                preDiffIO = PreDiffuser[i].GetOutput();
            }

            // this compensates for fact that we take 4 output taps at full volume
            // It also reduces the max value pushed into the delay line, since that it stored as an integer and can clip
            Gain(preDiffIO, 0.5, bufSize); 

            for (size_t i = 0; i < ZCOUNT; i++)
            {
                Copy(buf, preDiffIO, bufSize);
                Mix(buf, Delay[(i - 1 + ZCOUNT) % ZCOUNT].GetOutput(), Krt, bufSize);
                if (i == 0)
                {
                    lpPost.Process(buf, buf, bufSize);
                }
                Diffuser[2*i].Process(buf, bufSize);
                Diffuser[2*i+1].Process(Diffuser[2*i].GetOutput(), bufSize);
                Delay[i].Process(Diffuser[2*i+1].GetOutput(), bufSize);
            }
            
            ZeroBuffer(outputs[0], bufSize);
            Mix(outputs[0], Delay[0].GetOutput(), Wet, bufSize);
            Mix(outputs[0], Delay[2].GetOutput(), Wet, bufSize);
            Mix(outputs[0], inputs[0], Dry, bufSize);

            ZeroBuffer(outputs[1], bufSize);
            Mix(outputs[1], Delay[1].GetOutput(), Wet, bufSize);
            Mix(outputs[1], Delay[3].GetOutput(), Wet, bufSize);
            Mix(outputs[1], inputs[1], Dry, bufSize);
        }
        
    };
}
