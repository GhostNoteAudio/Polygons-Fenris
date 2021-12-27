#pragma once
#include "Polygons.h"
#include "blocks/WavReader.h"
#include "blocks/FFT.h"
#include "ParameterFenris.h"
#include "blocks/Interpolation.h"

using namespace Polygons;

constexpr bool powerOf2(int n)
{
    return (n & (n - 1)) == 0;
}

namespace Fenris
{
    template<int N>
    class CabEmulation
    {
        float kernel[N];
        int samplerate;
        bool kernelLoaded;
        unsigned int kernelSize;
        ConvFFT<N> conv;

    public:
        char irName[64];

    private:
        void SetIrName(const char* path)
        {
            int len = strlen(path);
            for (size_t i = len-2; i >= 0; i--)
            {
                if (path[i] == '/')
                {
                    strncpy(irName, &path[i+1], 64);
                    int filenameLen = strlen(irName);
                    irName[filenameLen - 4] = 0; // removes .wav extension
                    break;
                }
            }
        }

    public:
        CabEmulation(int samplerate) : conv(AUDIO_BLOCK_SAMPLES)
        {
            static_assert(powerOf2(N), "N must be power of 2");
            this->samplerate = samplerate;
            kernelLoaded = false;
            kernelSize = 0;
        }

        void SetParameter(int paramId, double value)
        {
            if (paramId == Parameter::CabScaleL)
            {
                
            }
        }

        void LoadImpulse(int index, float scale = 0)
        {
            kernelLoaded = false;
            uint8_t bufData[4096];
            auto path = Storage::GetFilePath("CabImpulses", 2);
            SetIrName(path);
            int size = Storage::GetFileSize(path);
            Serial.print("Path: ");
            Serial.println(path);
            Serial.print("Ir Name: ");
            Serial.println(irName);
            Storage::ReadFile(path, bufData, 4096);
            auto info = Polygons::GetWaveInfo(bufData, size);
            
            Serial.print("Bytes per sample: ");
            Serial.println(info.BytesPerSample);
            Serial.print("Channels: ");
            Serial.println(info.Channels);
            Serial.print("Samplecount: ");
            Serial.println(info.SampleCount);
            Serial.print("Samplerate: ");
            Serial.println(info.SampleRate);

            if (scale == 0)
                scale = info.SampleRate / (float)samplerate;
            
            ZeroBuffer(kernel, sizeof(kernel) / sizeof(float));
            GetWaveData(&info, kernel);
            kernelSize = ScaleInPlace(kernel, info.SampleCount, N, scale);
            conv.SetKernel(kernel, kernelSize);
            kernelLoaded = true;
        }

        void Process(float* input, float* output, int bufSize)
        {
            if (!kernelLoaded)
            {
                Serial.println("No kernel loaded");
                return;
            }
            conv.Process(input, output, bufSize);
        }
    };
}
