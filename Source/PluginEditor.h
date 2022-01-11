/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PlayerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PlayerAudioProcessorEditor (PlayerAudioProcessor&);
    ~PlayerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PlayerAudioProcessor& audioProcessor; 

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void changeText();

    std::unique_ptr<juce::FileChooser> chooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerAudioProcessorEditor)
};
