
#pragma once

#include "Constants.h"
#include "ParameterFenris.h"
#include "Utils.h"
#include "CabEmulation.h"
#include "StereoDelay.h"
#include "Utils.h"

namespace Fenris
{
	class Controller
	{
	private:
		CabEmulation<1024> CabLeft;
		CabEmulation<1024> CabRight;
		StereoDelay Delay;

		int samplerate;

		uint16_t parameters[Parameter::COUNT];

	public:
		Controller(int samplerate) : CabLeft(samplerate), CabRight(samplerate)
		{
			this->samplerate = samplerate;
			
		}

		void Init()
		{
			CabLeft.LoadImpulse(2, 1.0);
			CabRight.LoadImpulse(2, 1.0);
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
                case Parameter::CabIrL:                     return parameters[Parameter::CabIrL];
                case Parameter::CabIrR:                   	return parameters[Parameter::CabIrR];
				case Parameter::CabScaleL:                	return 50 + P(Parameter::CabScaleL);
                case Parameter::CabScaleR:                	return 50 + P(Parameter::CabScaleR);
				case Parameter::CabDelayL:                	return P(Parameter::CabDelayL) * 100;
				case Parameter::CabDelayR:                	return P(Parameter::CabDelayR) * 100;
				case Parameter::CabGainL:                	return -30 + P(Parameter::CabGainL) * 60;
				case Parameter::CabGainR:                	return -30 + P(Parameter::CabGainR) * 60;

				case Parameter::DelayTimeL:					return Polygons::Response2Dec(P(Parameter::DelayTimeL)) * 999.9;
				case Parameter::DelayTimeR:					return Polygons::Response2Dec(P(Parameter::DelayTimeR)) * 999.9;
				case Parameter::DelayFeedback:				return P(Parameter::DelayFeedback);
				case Parameter::DelayLow:					return 10 + P(Parameter::DelayLow) * 990;
				case Parameter::DelayHigh:					return 200 + Polygons::Response2Dec(P(Parameter::DelayHigh)) * 19800;
				case Parameter::DelayMix:					return P(Parameter::DelayMix);
				case Parameter::DelayXFeed:					return P(Parameter::DelayXFeed);
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
		}

		void ClearBuffers()
		{
		}

		void Process(float** inputs, float** outputs, int bufferSize)
		{
			/*for (int i = 0; i < bufferSize; i++)
			{
				outputs[0][i] = inputs[0][i];
				outputs[1][i] = inputs[1][i];
			}*/

			float tempL[64];
			float tempR[64];
			float* temps[2] = {tempL, tempR};
			
			CabLeft.Process(inputs[0], tempL, bufferSize);
			CabRight.Process(inputs[1], tempR, bufferSize);
			Delay.Process(temps, outputs, bufferSize);
		}
		
	private:
		double P(int para, int maxVal=1023)
		{
			auto idx = (int)para;
			return idx >= 0 && idx < Parameter::COUNT ? (parameters[idx] / (double)maxVal) : 0.0;
		}
	};
}
