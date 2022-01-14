#pragma once

#include "Polygons.h"
#include "ParameterFenris.h"
#include "ControllerFenris.h"
#include "blocks/Limiter.h"
#include "Utils.h"

namespace Fenris
{
    const int PRESET_COUNT = 7;

    float BufferInL[BUFFER_SIZE];
    float BufferInR[BUFFER_SIZE];
    float BufferOutL[BUFFER_SIZE];
    float BufferOutR[BUFFER_SIZE];

    Controller controller(SAMPLERATE);
    PolyOS os;

    uint16_t Presets[Parameter::COUNT * PRESET_COUNT];
    uint8_t currentPreset;

    void loadPreset(int number);
    void storePreset(int number);

    const char* ParameterNames[Parameter::COUNT];
    
    void setNames()
    {
        ParameterNames[Parameter::ShaperGain] = "Gain";
        ParameterNames[Parameter::ShaperCurve] = "Curve";
        ParameterNames[Parameter::ShaperPresence] = "Presence";
        ParameterNames[Parameter::ShaperDepth] = "Depth";
        ParameterNames[Parameter::ShaperBiasMod] = "Bias Mod";
        ParameterNames[Parameter::ShaperBiasSpeed] = "Bias Speed";
        ParameterNames[Parameter::ShaperGateThres] = "Gate Thres";
        ParameterNames[Parameter::ShaperGateRel] = "Gate Rel";
        ParameterNames[Parameter::CabIrL] = "Ir Left";
        ParameterNames[Parameter::CabIrR] = "Ir Right";
        ParameterNames[Parameter::CabScaleL] = "Scale Left";
        ParameterNames[Parameter::CabScaleR] = "Scale Right";
        ParameterNames[Parameter::CabDelayL] = "Delay L";
        ParameterNames[Parameter::CabDelayR] = "Delay R";
        ParameterNames[Parameter::CabGainL] = "Gain Left";
        ParameterNames[Parameter::CabGainR] = "Gain Right";
        ParameterNames[Parameter::EqLowF] = "Low Freq";
        ParameterNames[Parameter::EqHighF] = "High Freq";
        ParameterNames[Parameter::EqMid1F] = "Mid1 Freq";
        ParameterNames[Parameter::EqMid2F] = "Mid2 Freq";
        ParameterNames[Parameter::EqLowG] = "Low Gain";
        ParameterNames[Parameter::EqHighG] = "High Gain";
        ParameterNames[Parameter::EqMid1G] = "Mid1 Gain";
        ParameterNames[Parameter::EqMid2G] = "Mid2 Gain";
        ParameterNames[Parameter::LimiterThres] = "Threshold";
        ParameterNames[Parameter::LimiterBoost] = "Boost";
        ParameterNames[Parameter::LimiterRelease] = "Release";
        ParameterNames[Parameter::LimiterRatio] = "Ratio";
        ParameterNames[Parameter::EnhancerAmount] = "Amount";
        ParameterNames[Parameter::DelayTimeL] = "Len Left";
        ParameterNames[Parameter::DelayTimeR] = "Len Right";
        ParameterNames[Parameter::DelayFeedback] = "Feedback";
        ParameterNames[Parameter::DelayLow] = "Low";
        ParameterNames[Parameter::DelayHigh] = "High";
        ParameterNames[Parameter::DelayMix] = "Mix";
	    ParameterNames[Parameter::DelayXFeed] = "X-feed";
        ParameterNames[Parameter::ReverbDecay] = "Decay";
        ParameterNames[Parameter::ReverbDiffuse] = "Diffuse";
        ParameterNames[Parameter::ReverbBits] = "Bits";
        ParameterNames[Parameter::ReverbMix] = "Mix";
        ParameterNames[Parameter::ReverbSizeEarly] = "Size Early";
        ParameterNames[Parameter::ReverbSizeLate] = "Size Late";
        ParameterNames[Parameter::ReverbModulate] = "Modulate";
        ParameterNames[Parameter::ReverbTone] = "High";
        ParameterNames[Parameter::MasterInput] = "Input";
        ParameterNames[Parameter::MasterPan] = "Pan";
        ParameterNames[Parameter::MasterInGain] = "Input Gain";
        ParameterNames[Parameter::MasterOutGain] = "Output Gain";
        ParameterNames[Parameter::Active] = "Active";
    }

    void RegisterParams()
    {
        os.Register(Parameter::ShaperGain,          1023, Polygons::ControlMode::Encoded, 0+0, 4);
        os.Register(Parameter::ShaperCurve,         1023, Polygons::ControlMode::Encoded, 0+1, 4);
        os.Register(Parameter::ShaperPresence,      1023, Polygons::ControlMode::Encoded, 0+2, 4);
        os.Register(Parameter::ShaperDepth,         1023, Polygons::ControlMode::Encoded, 0+3, 4);
        os.Register(Parameter::ShaperBiasMod,       1023, Polygons::ControlMode::Encoded, 0+4, 4);
        os.Register(Parameter::ShaperBiasSpeed,     1023, Polygons::ControlMode::Encoded, 0+5, 4);
        os.Register(Parameter::ShaperGateThres,     1023, Polygons::ControlMode::Encoded, 0+6, 4);
        os.Register(Parameter::ShaperGateRel,       1023, Polygons::ControlMode::Encoded, 0+7, 4);

        os.Register(Parameter::CabIrL,              1023, Polygons::ControlMode::Encoded, 8+0, 4);
        os.Register(Parameter::CabIrR,              1023, Polygons::ControlMode::Encoded, 8+1, 4);
        os.Register(Parameter::CabScaleL,           1023, Polygons::ControlMode::Encoded, 8+2, 4);
        os.Register(Parameter::CabScaleR,           1023, Polygons::ControlMode::Encoded, 8+3, 4);
        os.Register(Parameter::CabDelayL,           1023, Polygons::ControlMode::Encoded, 8+4, 4);
        os.Register(Parameter::CabDelayR,           1023, Polygons::ControlMode::Encoded, 8+5, 4);
        os.Register(Parameter::CabGainL,            1023, Polygons::ControlMode::Encoded, 8+6, 4);
        os.Register(Parameter::CabGainR,            1023, Polygons::ControlMode::Encoded, 8+7, 4);

        os.Register(Parameter::EqLowF,              1023, Polygons::ControlMode::Encoded, 16+0, 4);
        os.Register(Parameter::EqHighF,             1023, Polygons::ControlMode::Encoded, 16+1, 4);
        os.Register(Parameter::EqMid1F,             1023, Polygons::ControlMode::Encoded, 16+2, 4);
        os.Register(Parameter::EqMid2F,             1023, Polygons::ControlMode::Encoded, 16+3, 4);
        os.Register(Parameter::EqLowG,              1023, Polygons::ControlMode::Encoded, 16+4, 4);
        os.Register(Parameter::EqHighG,             1023, Polygons::ControlMode::Encoded, 16+5, 4);
        os.Register(Parameter::EqMid1G,             1023, Polygons::ControlMode::Encoded, 16+6, 4);
        os.Register(Parameter::EqMid2G,             1023, Polygons::ControlMode::Encoded, 16+7, 4);

        os.Register(Parameter::LimiterThres,        1023, Polygons::ControlMode::Encoded, 24+0, 4);
        os.Register(Parameter::LimiterBoost,        1023, Polygons::ControlMode::Encoded, 24+1, 4);
        os.Register(Parameter::LimiterRelease,      1023, Polygons::ControlMode::Encoded, 24+2, 4);
        os.Register(Parameter::LimiterRatio,        1023, Polygons::ControlMode::Encoded, 24+3, 4);

        os.Register(Parameter::EnhancerAmount,      1023, Polygons::ControlMode::Encoded, 32+0, 4);

        os.Register(Parameter::DelayTimeL,          1023, Polygons::ControlMode::Encoded, 40+0, 4);
        os.Register(Parameter::DelayTimeR,          1023, Polygons::ControlMode::Encoded, 40+1, 4);
        os.Register(Parameter::DelayFeedback,       1023, Polygons::ControlMode::Encoded, 40+2, 4);
        os.Register(Parameter::DelayLow,            1023, Polygons::ControlMode::Encoded, 40+3, 4);
        os.Register(Parameter::DelayHigh,           1023, Polygons::ControlMode::Encoded, 40+4, 4);
        os.Register(Parameter::DelayMix,            1023, Polygons::ControlMode::Encoded, 40+5, 4);
	    os.Register(Parameter::DelayXFeed,          1023, Polygons::ControlMode::Encoded, 40+6, 4);

        os.Register(Parameter::ReverbDecay,         1023, Polygons::ControlMode::Encoded, 48+0, 4);
        os.Register(Parameter::ReverbDiffuse,       1023, Polygons::ControlMode::Encoded, 48+1, 4);
        os.Register(Parameter::ReverbBits,          1023, Polygons::ControlMode::Encoded, 48+2, 4);
        os.Register(Parameter::ReverbMix,           1023, Polygons::ControlMode::Encoded, 48+3, 4);
        os.Register(Parameter::ReverbSizeEarly,     1023, Polygons::ControlMode::Encoded, 48+4, 4);
        os.Register(Parameter::ReverbSizeLate,      1023, Polygons::ControlMode::Encoded, 48+5, 4);
        os.Register(Parameter::ReverbModulate,      1023, Polygons::ControlMode::Encoded, 48+6, 4);
        os.Register(Parameter::ReverbTone,          1023, Polygons::ControlMode::Encoded, 48+7, 4);

        os.Register(Parameter::MasterInput,         1023, Polygons::ControlMode::Encoded, 56+0, 4);
        os.Register(Parameter::MasterPan,           1023, Polygons::ControlMode::Encoded, 56+1, 4);
        os.Register(Parameter::MasterInGain,        1023, Polygons::ControlMode::Encoded, 56+2, 4);
        os.Register(Parameter::MasterOutGain,       1023, Polygons::ControlMode::Encoded, 56+3, 4);

        os.Register(Parameter::Active,              1023, Polygons::ControlMode::DigitalToggle, 9, 4);
    }

    inline void getPageName(int page, char* dest)
    {
        if (page == 0)
            strcpy(dest, "Shaper");
        else if (page == 1)
            strcpy(dest, "Cabinet");
        else if (page == 2)
            strcpy(dest, "EQ");
        else if (page == 3)
            strcpy(dest, "Limiter");
        else if (page == 4)
            strcpy(dest, "Enhancer");
        else if (page == 5)
            strcpy(dest, "Delay");
        else if (page == 6)
            strcpy(dest, "Reverb");
        else if (page == 7)
            strcpy(dest, "Master");
        else
            strcpy(dest, "");
    }

    inline void getParameterName(int paramId, char* dest)
    {
        if (paramId >= 0)
            strcpy(dest, ParameterNames[paramId]);
        else
            strcpy(dest, "");
    }

    inline void getParameterDisplay(int paramId, char* dest)
    {
        double val = controller.GetScaledParameter(paramId);
        if (paramId == -1)
        {
            strcpy(dest, "");
        }
        else if (paramId == Parameter::EnhancerAmount)
        {
            sprintf(dest, "%d%%", (int)(val*100));
        }
        else if (paramId == Parameter::DelayTimeL || paramId == Parameter::DelayTimeR)
        {
            sprintf(dest, "%.0f ms", val);
        }
        else if (paramId == Parameter::DelayFeedback || paramId == Parameter::DelayXFeed || paramId == Parameter::DelayMix)
        {
            sprintf(dest, "%d%%", (int)(val*100));
        }
        else if (paramId == Parameter::DelayLow || paramId == Parameter::DelayHigh)
        {
            if (val < 1000)
                sprintf(dest, "%dHz", (int)val);
            else
                sprintf(dest, "%.1fKHz", val*0.001);
        }
        else if (paramId == Parameter::ReverbDecay)
        {
            sprintf(dest, "%.1fs", val);
        }
        else if (paramId == Parameter::ReverbDiffuse || paramId == Parameter::ReverbSizeEarly || paramId == Parameter::ReverbSizeLate || paramId == Parameter::ReverbTone || paramId == Parameter::ReverbMix || paramId == Parameter::ReverbModulate)
        {
            sprintf(dest, "%d%%", (int)(val*100));
        }
        else
        {
            sprintf(dest, "%.2f", val);
        }
    }

    inline void setPresetLed()
    {
        // changes the different RGB colours of the LED to indicate preset number
        Polygons::pushDigital(0, (currentPreset + 1) & 0b001);
        Polygons::pushDigital(1, (currentPreset + 1) & 0b010);
        Polygons::pushDigital(2, (currentPreset + 1) & 0b100);
    }

    inline void setActiveLed()
    {
       bool active = controller.GetScaledParameter(Parameter::Active) == 1;
        // Uses the Red LED to indicate active and freeze states
        Polygons::pushDigital(3, active);
    }

    inline void setIOConfig()
    {
        int gainIn = (int8_t)controller.GetScaledParameter(Parameter::MasterInGain) * 2;
        int gainOut = (int8_t)controller.GetScaledParameter(Parameter::MasterOutGain);
        Polygons::codec.analogInGain(gainIn, gainIn);
        Polygons::codec.lineOutGain(gainOut, gainOut, false);
        Polygons::codec.headphoneGain(gainOut, gainOut, false);
    }

    inline bool handleUpdate(Polygons::ParameterUpdate* update)
    {
       return false;
    }

    inline void setParameter(uint8_t paramId, uint16_t value)
    {
        controller.SetParameter(paramId, (uint16_t)value);
        setActiveLed();
        if (paramId >= Parameter::MasterInput && paramId <= Parameter::MasterOutGain)
            setIOConfig();
    }
    
    void audioCallback(int32_t** inputs, int32_t** outputs)
    {
        float scaler = (float)(1.0 / (double)SAMPLE_32_MAX);
        for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            BufferInL[i] = inputs[0][i] * scaler;
            BufferInR[i] = inputs[1][i] * scaler;
        }

        float* ins[2] = {BufferInL, BufferInR};
        float* outs[2] = {BufferOutL, BufferOutR};
        controller.Process(ins, outs, AUDIO_BLOCK_SAMPLES);

        for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            outputs[0][i] = (int)(BufferOutL[i] * SAMPLE_32_MAX);
            outputs[1][i] = (int)(BufferOutR[i] * SAMPLE_32_MAX);
        }
    }

    void loadPreset(int number)
    {
    }

    void storePreset(int number)
    {

    }

    inline void LoadPresetsSD()
    {
    }

    inline void drawLimiter()
    {
        if (os.SelectedPage == 3) // Limiter page, draw indicators
        {
            auto canvas = getCanvas();
            canvas->drawRect(1, 43, 100, 8, 1);
            canvas->drawRect(128+1, 43, 100, 8, 1);

            float gainFrac = max((Limiter::InputGainDb+40.0)/40.0, 0.0);
            float reductionFrac = min(Limiter::GainReductionDb/20.0,1);

            canvas->fillRect(1, 43, gainFrac * 100, 8, 1);
            canvas->fillRect(128+1, 43, reductionFrac * 100, 8, 1);

            canvas->setTextColor(1);
            canvas->setCursor(1, 40);
            canvas->println("In Gain");
            canvas->setCursor(65, 40);
            canvas->println(Limiter::InputGainDb, 1);

            canvas->setCursor(129, 40);
            canvas->println("Reduction");
            canvas->setCursor(193, 40);
            canvas->println(Limiter::GainReductionDb, 1);
        }
    }

    inline void start()
    {
        Serial.println("Starting up - waiting for controller signal...");
        //os.waitForControllerSignal();
        setNames();
        RegisterParams();

       for (size_t i = 0; i < Parameter::COUNT * PRESET_COUNT; i++)
           Presets[i] = 0;
        
        controller.Init();

        os.HandleUpdateCallback = handleUpdate;
        os.SetParameterCallback = setParameter;
        os.CustomDrawCallback = drawLimiter;
        os.PageCount = 8;
        os.menu.getPageName = getPageName;
        os.menu.getParameterName = getParameterName;
        os.menu.getParameterDisplay = getParameterDisplay;
        
        LoadPresetsSD();
        loadPreset(0);
        controller.SetParameter(Parameter::Active, 1);
        setActiveLed();
        i2sAudioCallback = audioCallback;
    }

    inline void loop()
    {
        os.loop();
    }
}
