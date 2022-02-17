#pragma once

namespace Fenris
{
    class Parameter
    {
    public:
        static const int ShaperGain = 0;
        static const int ShaperCurve = 1;
        static const int ShaperPresence = 2;
        static const int ShaperDepth = 3;
        static const int ShaperBiasMod = 4;
        static const int ShaperBiasSpeed = 5;
        static const int ShaperGateThres = 6;
        static const int ShaperGateRel = 7;

        static const int CabIrL = 8;
        static const int CabIrR = 9;
        static const int CabScaleL = 10;
        static const int CabScaleR = 11;
        static const int CabDelayL = 12;
        static const int CabDelayR = 13;
        static const int CabGainL = 14;
        static const int CabGainR = 15;

        static const int EqLowF = 16;
        static const int EqHighF = 17;
        static const int EqMid1F = 18;
        static const int EqMid2F = 19;
        static const int EqLowG = 20;
        static const int EqHighG = 21;
        static const int EqMid1G = 22;
        static const int EqMid2G = 23;

        static const int LimiterThres = 24;
        static const int LimiterBoost = 25;
        static const int LimiterRelease = 26;
        static const int LimiterRatio = 27;

        static const int EnhancerAmount = 28;

        static const int DelayTimeL = 29;
        static const int DelayTimeR = 30;
        static const int DelayFeedback = 31;
        static const int DelayLow = 32;
        static const int DelayHigh = 33;
        static const int DelayMix = 34;
	    static const int DelayXFeed = 35;

        static const int ReverbDecay = 36;
        static const int ReverbDiffuse = 37;
        static const int ReverbBits = 38;
        static const int ReverbMix = 39;
        static const int ReverbSizeEarly = 40;
        static const int ReverbSizeLate = 41;
        static const int ReverbModulate = 42;
        static const int ReverbTone = 43;
        
        static const int MasterInput = 44;
        static const int MasterPan = 45;
        static const int MasterInGain = 46;
        static const int MasterOutGain = 47;

        static const int Active = 48;

        static const int COUNT = 49;
    };
}
