#pragma once
#include "Utils.h"
#include "blocks/FFT.h"
#include "arm_math.h"
#include "ParameterFenris.h"
#include "blocks/Convolve.h"
#include "Buffers.h"
#include "blocks/DelayBlock.h"

using namespace Polygons;

namespace Fenris
{
    class Enhancer
    {
        static const int WINDOW_SIZE = 64;

        float f0[WINDOW_SIZE];
        float f1[WINDOW_SIZE];
        float f2[WINDOW_SIZE];
        float f3[WINDOW_SIZE];
        Convolver<128> conv[4];
        DelayBlock<2048> delays[4];

        int Samplerate;
        float Size;
    public:
        Enhancer(int samplerate)
        {
            Size = 1;
            Samplerate = samplerate;
        }

        inline void SetParameter(int paramId, double value)
        {
            if (paramId == Parameter::EnhancerAmount)
            {
                Size = value;
            }
        }

        inline void Init()
        {
            MakeSincFilter(f0, WINDOW_SIZE, 0, 200, Samplerate);
            MakeSincFilter(f1, WINDOW_SIZE, 201, 800, Samplerate);
            MakeSincFilter(f2, WINDOW_SIZE, 801, 3200, Samplerate);
            MakeSincFilter(f3, WINDOW_SIZE, 3201, Samplerate/2, Samplerate);
            conv[0].Init(f0, WINDOW_SIZE, BUFFER_SIZE);
            conv[1].Init(f1, WINDOW_SIZE, BUFFER_SIZE);
            conv[2].Init(f2, WINDOW_SIZE, BUFFER_SIZE);
            conv[3].Init(f3, WINDOW_SIZE, BUFFER_SIZE);
        }

        inline void Process(float** inputs, float** outputs, int bufSize)
        {
            if (Size == 0)
            {
                Copy(outputs[0], inputs[0], bufSize);
                Copy(outputs[1], inputs[1], bufSize);
                return;
            }

            // We only affect the left channel and pass the right through unchanged
            
            auto b = Buffers::Request();
            auto buf = b.Ptr;
            ZeroBuffer(outputs[0], bufSize);
            auto db3gain = DB2gain(0.5); // slight gain compensation, because the windowing of the filters cuts some energy out of them

            conv[0].Process(inputs[0], buf, bufSize);
            delays[0].write(buf, bufSize);
            delays[0].read(buf, (int)(Size*253), bufSize);
            Mix(outputs[0], buf, db3gain, bufSize);
            
            conv[1].Process(inputs[0], buf, bufSize);
            delays[1].write(buf, bufSize);
            delays[1].read(buf, (int)(Size*575), bufSize);
            Mix(outputs[0], buf, db3gain, bufSize);

            conv[2].Process(inputs[0], buf, bufSize);
            delays[2].write(buf, bufSize);
            delays[2].read(buf, (int)(Size*1089), bufSize);
            Mix(outputs[0], buf, db3gain, bufSize);

            conv[3].Process(inputs[0], buf, bufSize);
            delays[3].write(buf, bufSize);
            delays[3].read(buf, (int)(Size*1395), bufSize);
            Mix(outputs[0], buf, db3gain, bufSize);

            Copy(outputs[1], inputs[1], bufSize);
            delays[0].updatePtr(bufSize);
            delays[1].updatePtr(bufSize);
            delays[2].updatePtr(bufSize);
            delays[3].updatePtr(bufSize);
        }
    };
}
