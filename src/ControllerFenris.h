
#pragma once

#include "Constants.h"
#include "ParameterFenris.h"
#include "Utils.h"
#include "CabEmulation.h"
#include "StereoDelay.h"
#include "Utils.h"
#include "ZzzzRev.h"
#include "Enhancer.h"
#include "Eq.h"
#include "blocks/Limiter.h"
#include "Shaper.h"

namespace Fenris
{
	enum class InputMode
	{
		Left = 0,
		Right,
		Sum
	};

	class Controller
	{
	private:
		CabEmulation<1024> CabLeft;
		CabEmulation<1024> CabRight;
		ZzzzRev Reverb;
		StereoDelay Delay;
		Enhancer Enhance;
		Polygons::Limiter Limiter;
		Eq Equalizer;
		Shaper Shape;

		int samplerate;
		InputMode inputMode;
		float inGain;
		float outGain;
		float pan;

		uint16_t parameters[Parameter::COUNT];

	public:
		Controller(int samplerate) : 
			CabLeft(samplerate), 
			CabRight(samplerate), 
			Reverb(samplerate), 
			Enhance(samplerate), 
			Limiter(samplerate),
			Equalizer(samplerate),
			Shape(samplerate)
		{
			this->samplerate = samplerate;
			inputMode = InputMode::Left;
			inGain = 1.0;
			outGain = 1.0;
			pan = 0.5;
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
				case Parameter::ShaperGain:                	return P(param);
				case Parameter::ShaperCurve:               	return (int)(P(param)*1.999);
				case Parameter::ShaperPresence:            	return P(param);
				case Parameter::ShaperDepth:            	return P(param);
				case Parameter::ShaperBiasMod:            	return P(param);
				case Parameter::ShaperBiasSpeed:           	return P(param);
				case Parameter::ShaperGateThres:           	return -80 + P(param)*80;
				case Parameter::ShaperGateRel:            	return P(param) * 500;

                case Parameter::CabIrL:                     return parameters[param];
                case Parameter::CabIrR:                   	return parameters[param];
				case Parameter::CabScaleL:                	return 50 + P(param);
                case Parameter::CabScaleR:                	return 50 + P(param);
				case Parameter::CabDelayL:                	return P(param) * 100;
				case Parameter::CabDelayR:                	return P(param) * 100;
				case Parameter::CabGainL:                	return -30 + P(param) * 60;
				case Parameter::CabGainR:                	return -30 + P(param) * 60;

				case Parameter::EqLowF:                  	return 40 + P(param) * 460;
				case Parameter::EqHighF:                  	return 2000 + P(param) * 14000;
				case Parameter::EqMid1F:                  	return 300 + P(param) * 1700;
				case Parameter::EqMid2F:                  	return 1000 + P(param) * 4000;
				case Parameter::EqLowG:                  	return -12 + P(param) * 24;
				case Parameter::EqHighG:                  	return -12 + P(param) * 24;
				case Parameter::EqMid1G:                  	return -12 + P(param) * 24;
				case Parameter::EqMid2G:                  	return -12 + P(param) * 24;

				case Parameter::LimiterThres:             	return -P(param) * 12;
				case Parameter::LimiterBoost:              	return P(param) * 30;
				case Parameter::LimiterRelease:            	return (int)(P(param) * 100);
				case Parameter::LimiterRatio:             	return P(param);

				case Parameter::EnhancerAmount:				return P(param);

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

				case Parameter::MasterInput:				return (int)(P(param) * 2.999);
				case Parameter::MasterPan:					return P(param);
				case Parameter::MasterInGain:				return -20 + P(param) * 40;
				case Parameter::MasterOutGain:				return -20 + P(param) * 40;
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
			Equalizer.SetParameter(param, scaled);
			Shape.SetParameter(param, scaled);

			if (param == Parameter::MasterInput)
				inputMode = (InputMode)(int)scaled;
			if (param == Parameter::MasterPan)
				pan = scaled;
			if (param == Parameter::MasterInGain)
				inGain = DB2gain(scaled);
			if (param == Parameter::MasterOutGain)
				outGain = DB2gain(scaled);
		}

		void ClearBuffers()
		{
		}

		void Process(float** inputs, float** outputs, int bufferSize)
		{
			auto tempA = Buffers::Request();
			auto tempB = Buffers::Request();
			auto bufA = tempA.Ptr;
			auto bufB = tempB.Ptr;

			float* input = 0;
			if (inputMode == InputMode::Left)
				input = inputs[0];
			else if (inputMode == InputMode::Right)
				input = inputs[1];
			else
			{
				Copy(bufA, inputs[0], bufferSize);
				Mix(bufA, inputs[1], 1.0, bufferSize);
				input = bufA;
			}

			Gain(input, inGain, bufferSize);

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
			//Limiter.Process(inputs[0], outputs[0], bufferSize);
			//Equalizer.Process(inputs[0], outputs[0], bufferSize);
			Shape.Process(input, outputs[0], bufferSize);
			Copy(outputs[1], outputs[0], bufferSize);
			Gain(outputs[0], outGain, bufferSize);
			Gain(outputs[1], outGain, bufferSize);
		}
		
	private:
		double P(int para, int maxVal=1023)
		{
			auto idx = (int)para;
			return idx >= 0 && idx < Parameter::COUNT ? (parameters[idx] / (double)maxVal) : 0.0;
		}
	};
}
