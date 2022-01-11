/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PlayerAudioProcessorEditor::PlayerAudioProcessorEditor(PlayerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::dodgerblue);
    playButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::cornflowerblue);
    stopButton.setEnabled(false);    
}

PlayerAudioProcessorEditor::~PlayerAudioProcessorEditor()
{
}

//==============================================================================
void PlayerAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    g.fillAll(juce::Colours::lightblue);
}

void PlayerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    openButton.setBounds(10, 10, getWidth() - 20, 20);
    playButton.setBounds(10, 40, getWidth() - 20, 20);
    stopButton.setBounds(10, 70, getWidth() - 20, 20);
}

void PlayerAudioProcessorEditor::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...", juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                auto* reader = audioProcessor.formatManager.createReaderFor(file);

                if (reader != nullptr)
                {
                    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                    audioProcessor.transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                    playButton.setEnabled(true);
                    audioProcessor.readerSource.reset(newSource.release());
                }
            }
        });
}

void PlayerAudioProcessorEditor::playButtonClicked()
{                                                              
    if (audioProcessor.state == PlayerAudioProcessor::TransportState::Playing)
    {
        audioProcessor.changeState(PlayerAudioProcessor::TransportState::Pausing);
        playButton.setButtonText("Resume");
        stopButton.setButtonText("Return to Zero");
    }
    else
    {
        audioProcessor.changeState(PlayerAudioProcessor::TransportState::Starting);
        playButton.setButtonText("Pause");
        stopButton.setButtonText("Stop");
        stopButton.setEnabled(true);
    }
}

void PlayerAudioProcessorEditor::stopButtonClicked()
{
    if (audioProcessor.state == PlayerAudioProcessor::TransportState::Paused)
    {
        audioProcessor.changeState(PlayerAudioProcessor::TransportState::Stopped);
        playButton.setButtonText("Play");
        stopButton.setButtonText("Stop");
        stopButton.setEnabled(false);
    }
    else
    {
        audioProcessor.changeState(PlayerAudioProcessor::TransportState::Stopping);
        playButton.setButtonText("Play");
        stopButton.setButtonText("Stop");
        stopButton.setEnabled(false);
    }
}


