#pragma once

#include "PluginProcessor.h"

namespace audio_plugin
{
    class PluginEditor : public juce::AudioProcessorEditor, public juce::Timer
                               //public juce::Slider::Listener
    {
    public:
        explicit PluginEditor (PluginProcessor&);
        ~PluginEditor() override;
        void paint (juce::Graphics&) override;
        void resized() override;
        //void sliderValueChanged (juce::Slider* slider) override {}

    void timerCallback() override {
        resized();
    }
    private:
        // This reference is provided as a quick way for your editor to
        // access the processor object that created it.
        PluginProcessor& processorRef;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
    };

} // namespace gluec