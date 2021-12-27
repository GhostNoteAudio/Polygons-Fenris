#pragma once

#include "Constants.h"
#include "blocks/ModulatedDelay.h"
#include "blocks/Biquad.h"

namespace Fenris
{
    class StereoDelay
    {
        float bufL[BUFFER_SIZE];
        float bufR[BUFFER_SIZE];
        Polygons::Biquad lpL;
        Polygons::Biquad lpR;
        Polygons::Biquad hpL;
        Polygons::Biquad hpR;
        Polygons::ModulatedDelay<FS_MAX, BUFFER_SIZE> delayLeft;
        Polygons::ModulatedDelay<FS_MAX, BUFFER_SIZE> delayRight;
    public:
        float Feedback;
        float FeedbackL;
        float FeedbackR;
        float XFeed;
        float MixAmount;

        StereoDelay() : lpL(Polygons::Biquad::FilterType::LowPass, SAMPLERATE),
                        lpR(Polygons::Biquad::FilterType::LowPass, SAMPLERATE),
                        hpL(Polygons::Biquad::FilterType::HighPass, SAMPLERATE),
                        hpR(Polygons::Biquad::FilterType::HighPass, SAMPLERATE)
        {
            delayLeft.SampleDelay = 9000;
            delayRight.SampleDelay = 9000;
            Feedback = FeedbackL = FeedbackR = 0.6;
            XFeed = 0.0;
            MixAmount = 1.0;
            lpL.Frequency = 1000;
            lpR.Frequency = 1000;
            hpL.Frequency = 10;
            hpR.Frequency = 10;
        }

        void SetParameter(int paramId, double value)
        {
            Serial.print("Setting param ");
            Serial.print(paramId);
            Serial.print(" to ");
            Serial.println(value);

            if (paramId == Parameter::DelayTimeL)
            {
                delayLeft.SampleDelay = value * SAMPLERATE * 0.001; // ms to samples
            }
            if (paramId == Parameter::DelayTimeR)
            {
                delayRight.SampleDelay = value * SAMPLERATE * 0.001; // ms to samples
            }
            if (paramId == Parameter::DelayFeedback)
            {
                Feedback = value;
            }
            if (paramId == Parameter::DelayLow)
            {
                hpL.Frequency = value;
                hpR.Frequency = value;
            }
            if (paramId == Parameter::DelayHigh)
            {
                lpL.Frequency = value;
                lpR.Frequency = value;
            }
            if (paramId == Parameter::DelayMix)
            {
                MixAmount = value;
            }
            if (paramId == Parameter::DelayXFeed)
            {
                XFeed = value;
            }

            UpdateAll();
        }

        void UpdateAll()
        {
            FeedbackL = Feedback;
            FeedbackR = std::pow(Feedback, delayRight.SampleDelay / (double)delayLeft.SampleDelay);

            lpL.Update();
            lpR.Update();
            hpL.Update();
            hpR.Update();
            XFeed = Clip(XFeed, 0.0, 1.0);
            MixAmount = Clip(MixAmount, 0.0, 1.0);
        }

        void Process(float** inputs, float** outputs, int bufSize)
        {
            auto dl = delayLeft.GetOutput();
            auto dr = delayRight.GetOutput();

            for (int i = 0; i < bufSize; i++)
            {
                outputs[0][i] = inputs[0][i] + dl[i] * MixAmount;
                outputs[1][i] = inputs[1][i] + dr[i] * MixAmount;
            }

            float feedbackLL = FeedbackL * (1-XFeed);
            float feedbackLR = FeedbackL * (XFeed);
            float feedbackRR = FeedbackR * (1-XFeed);
            float feedbackRL = FeedbackR * (XFeed);
            
            Copy(bufL, inputs[0], bufSize);
            Copy(bufR, inputs[1], bufSize);

            Mix(bufL, dl, feedbackLL, bufSize);
            Mix(bufL, dr, feedbackLR, bufSize);
            Mix(bufR, dr, feedbackRR, bufSize);
            Mix(bufR, dl, feedbackRL, bufSize);
            
            lpL.Process(bufL, bufL, bufSize);
            hpL.Process(bufL, bufL, bufSize);
            lpR.Process(bufR, bufR, bufSize);
            hpR.Process(bufR, bufR, bufSize);
            delayLeft.Process(bufL, bufSize);
            delayRight.Process(bufR, bufSize);
        }
    };
}
