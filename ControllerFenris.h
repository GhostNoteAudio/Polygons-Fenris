
#pragma once

#include "Constants.h"
#include "ParameterFenris.h"
#include "Utils.h"
#include "CabEmulation.h"
#include "StereoDelay.h"
#include "Utils.h"
#include "ZzzzRev.h"
#include "Enhancer.h"
#include "blocks/Limiter.h"

namespace Fenris
{
	class Controller
	{
	private:
		CabEmulation<1024> CabLeft;
		CabEmulation<1024> CabRight;
		ZzzzRev Reverb;
		StereoDelay Delay;
		Enhancer Enhance;
		Polygons::Limiter Limiter;

		int samplerate;

		uint16_t parameters[Parameter::COUNT];

	public:
		Controller(int samplerate) : CabLeft(samplerate), CabRight(samplerate), Reverb(samplerate), Enhance(samplerate), Limiter(samplerate)
		{
			this->samplerate = samplerate;			
		}

		void Init()
		{
			CabLeft.LoadImpulse(2, 1.0);
			CabRight.LoadImpulse(2, 1.0);
			Enhance.Init();
		}

		int GetSamplerate()
		{
			return samplerate;
		}

		uint16_t* GetAllParameters()
		{
			return parameters;
			
		}

		double GetScaledParameter(int param)
		{
			switch (param)
			{
                case Parameter::CabIrL:                     return parameters[param];
                case Parameter::CabIrR:                   	return parameters[param];
				case Parameter::CabScaleL:                	return 50 + P(param);
                case Parameter::CabScaleR:                	return 50 + P(param);
				case Parameter::CabDelayL:                	return P(param) * 100;
				case Parameter::CabDelayR:                	return P(param) * 100;
				case Parameter::CabGainL:                	return -30 + P(param) * 60;
				case Parameter::CabGainR:                	return -30 + P(param) * 60;

				case Parameter::LimiterThres:             	return -P(param) * 12;
				case Parameter::LimiterBoost:              	return P(param) * 30;
				case Parameter::LimiterRelease:            	return P(param) * 100;
				case Parameter::LimiterRatio:             	return P(param);

				case Parameter::DelayTimeL:					return Polygons::Response2Dec(P(param)) * 499.9;
				case Parameter::DelayTimeR:					return Polygons::Response2Dec(P(param)) * 499.9;
				case Parameter::DelayFeedback:				return P(param);
				case Parameter::DelayLow:					return 10 + P(param) * 990;
				case Parameter::DelayHigh:					return 200 + Polygons::Response2Dec(P(param)) * 19800;
				case Parameter::DelayMix:					return P(param);
				case Parameter::DelayXFeed:					return P(param);

				case Parameter::ReverbDecay:				return P(param) * 30;
				case Parameter::ReverbDiffuse:				return P(param);
				case Parameter::ReverbBits:					return (int)(1 + P(param) * 15.99);
				case Parameter::ReverbMix:					return P(param);
				case Parameter::ReverbSizeEarly:			return 0.1 + P(param) * 0.9;
				case Parameter::ReverbSizeLate:				return 0.1 + P(param) * 0.9;
				case Parameter::ReverbModulate:				return P(param);
				case Parameter::ReverbTone:					return P(param);

				case Parameter::EnhancerAmount:				return P(param);
			}
			return parameters[param];
		}

		void SetParameter(int param, uint16_t value)
		{
			parameters[param] = value;
			auto scaled = GetScaledParameter(param);
			CabLeft.SetParameter(param, scaled);
			CabRight.SetParameter(param, scaled);
			Delay.SetParameter(param, scaled);
			Reverb.SetParameters(param, scaled);
			Enhance.SetParameter(param, scaled);
			Limiter.SetParameter(param-Parameter::LimiterThres, scaled);
		}

		void ClearBuffers()
		{
		}

		void Process(float** inputs, float** outputs, int bufferSize)
		{
			//auto tempL = Buffers::Request();
			//auto tempR = Buffers::Request();
			//float* temps[2] = {tempL.Ptr, tempR.Ptr};

			//auto tempL2 = Buffers::Request();
			//auto tempR2 = Buffers::Request();
			//float* temps2[2] = {tempL2.Ptr, tempR2.Ptr};

			//CabLeft.Process(inputs[0], tempL, bufferSize);
			//CabRight.Process(inputs[1], tempR, bufferSize);
			//Copy(temps[0], inputs[0], bufferSize);
			//Copy(temps[1], inputs[1], bufferSize);
			//Delay.Process(temps, temps2, bufferSize);
			//Reverb.Process(temps, outputs, bufferSize);
			//Enhance.Process(temps, outputs, bufferSize);
			Limiter.Process(inputs[0], outputs[0], bufferSize);
			Copy(outputs[1], outputs[0], bufferSize);
		}
		
	private:
		double P(int para, int maxVal=1023)
		{
			auto idx = (int)para;
			return idx >= 0 && idx < Parameter::COUNT ? (parameters[idx] / (double)maxVal) : 0.0;
		}
	};
}
