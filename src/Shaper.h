#pragma once
#include "Polygons.h"
#include "Utils.h"
#include "blocks/Biquad.h"
#include "ParameterFenris.h"

using namespace Polygons;

namespace Fenris
{
    class Shaper
    {
        Biquad Presence;
        Biquad Depth;

        int samplerate;
        int curve;
        float gain;
        float biasMod;
        float biasSpeed;
        float biasValue;

    public:
        inline Shaper(int samplerate) : 
            Presence(Biquad::FilterType::HighShelf, samplerate), 
            Depth(Biquad::FilterType::LowShelf, samplerate)
        {
            this->samplerate = samplerate;
            curve = 0;
            gain = 1.0;
            biasMod = 0.0;
            biasSpeed = 0.001;
            biasValue = 0;

            Depth.Frequency = 150;
            Depth.SetGainDb(0);
            Depth.SetQ(0.7);
            Depth.Update();

            Presence.Frequency = 8000;
            Presence.SetGainDb(0);
            Presence.SetQ(0.7);
            Presence.Update();
        }

        inline void SetParameter(int paramId, double value)
        {
            /*
            case Parameter::ShaperGain:                	return P(param);
				case Parameter::ShaperCurve:               	return (int)(P(param)*1.999);
				case Parameter::ShaperPresence:            	return P(param);
				case Parameter::ShaperDepth:            	return P(param);
				case Parameter::ShaperBiasMod:            	return P(param);
				case Parameter::ShaperBiasSpeed:           	return P(param);
				case Parameter::ShaperGateThres:           	return -80 + P(param)*80;
				case Parameter::ShaperGateRel:            	return P(param) * 500;
            */
            if (paramId == Parameter::ShaperGain)
                gain = 1 + value * 19;
            else if (paramId == Parameter::ShaperPresence)
                Presence.SetGainDb(-10 + value * 20);
            else if (paramId == Parameter::ShaperDepth)
                Depth.SetGainDb(-10 + value * 20);
            else if (paramId == Parameter::ShaperBiasMod)
                biasMod = value * 1.5;
            else if (paramId == Parameter::ShaperBiasSpeed)
                biasSpeed = 0.00003 + 0.0005 * value;

            Presence.Update();
            Depth.Update();
        }

        inline void Process(float* input, float* output, int bufSize)
        {
            auto b1 = Buffers::Request();
            auto b2 = Buffers::Request();
            auto buf = b1.Ptr;
            auto bufBias = b2.Ptr;

            for (int i = 0; i < bufSize; i++)
            {
                float val = fabsf(input[i]);
                biasValue = biasValue * (1-biasSpeed) + val * biasSpeed;
                bufBias[i] = biasValue * biasMod;
            }

            Depth.Process(input, buf, bufSize);

            if (curve == 0)
            {
                for (int i = 0; i < bufSize; i++)
                    buf[i] = tanhf((buf[i] + bufBias[i]) * gain);
            }
            else if (curve == 1)
            {
                for (int i = 0; i < bufSize; i++)
                    buf[i] = Clip1((buf[i] + bufBias[i]) * gain);
            }

            Presence.Process(buf, buf, bufSize);
            Copy(output, buf, bufSize);
        }
    };
}
