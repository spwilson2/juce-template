#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>

namespace audio_plugin
{

    int kWidth = 240;
    int kHeight = 200;

    PluginEditor::PluginEditor (PluginProcessor& p)
        : AudioProcessorEditor (&p), processorRef (p)
    {
        startTimer(100);
        setSize(kWidth, kHeight);
    }


    PluginEditor::~PluginEditor()
    {
        setLookAndFeel (nullptr);
    }

    //void PluginEditor::sliderValueChanged (juce::Slider* slider) {
    //}

    void PluginEditor::paint (juce::Graphics& g)
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }

    // TODO Juce linespacer class? (Draws a line for the full size?

    void PluginEditor::resized()
    {
        repaint();
    }

} // namespace gluec