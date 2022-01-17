/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PlayerAudioProcessorEditor::PlayerAudioProcessorEditor (PlayerAudioProcessor& p)
    : state (TransportState::Stopped), AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    addAndMakeVisible (&openButton);
    openButton.setButtonText ("Open...");
    openButton.onClick = [this] { openButtonClicked (); };

    addAndMakeVisible (&playButton);
    playButton.setButtonText ("Play");
    playButton.onClick = [this] { playButtonClicked (); };
    playButton.setColour (juce::TextButton::buttonColourId, juce::Colours::dodgerblue);
    playButton.setEnabled (false);

    addAndMakeVisible (&stopButton);
    stopButton.setButtonText ("Stop");
    stopButton.onClick = [this] { stopButtonClicked (); };
    stopButton.setColour (juce::TextButton::buttonColourId, juce::Colours::cornflowerblue);
    stopButton.setEnabled (false);

    audioProcessor.transportSource.addChangeListener (this);
}

PlayerAudioProcessorEditor::~PlayerAudioProcessorEditor ()
{
}

//==============================================================================
void PlayerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::lightblue);
}

void PlayerAudioProcessorEditor::resized ()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    openButton.setBounds (10, 10, getWidth () - 20, 20);
    playButton.setBounds (10, 40, getWidth () - 20, 20);
    stopButton.setBounds (10, 70, getWidth () - 20, 20);
}

void PlayerAudioProcessorEditor::changeState (TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
            case TransportState::Stopped:
                playButton.setButtonText ("Play");
                stopButton.setButtonText ("Stop");
                stopButton.setEnabled (false);
                audioProcessor.transportSource.setPosition (0.0);
                break;

            case TransportState::Starting:
                audioProcessor.transportSource.start ();
                break;

            case TransportState::Playing:
                playButton.setButtonText ("Pause");
                stopButton.setButtonText ("Stop");
                stopButton.setEnabled (true);
                break;

            case TransportState::Pausing:
                audioProcessor.transportSource.stop ();
                break;

            case TransportState::Paused:
                playButton.setButtonText ("Resume");
                stopButton.setButtonText ("Return to Zero");
                break;

            case TransportState::Stopping:
                audioProcessor.transportSource.stop ();
                break;
        }
    }
}

void PlayerAudioProcessorEditor::changeListenerCallback (juce::ChangeBroadcaster* source) 
{
    if (source == &audioProcessor.transportSource)
    {
        if (audioProcessor.transportSource.isPlaying ())
            changeState (TransportState::Playing);
        else if ((state == TransportState::Stopping) || (state == TransportState::Playing))
            changeState (TransportState::Stopped);
        else if (TransportState::Pausing == state)
            changeState (TransportState::Paused);
    }
}

void PlayerAudioProcessorEditor::openButtonClicked ()
{
    chooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...", juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync (chooserFlags, [this](const juce::FileChooser& fc)
                          {
                              auto file = fc.getResult ();

                              if (file != juce::File{})
                              {
                                  auto* reader = audioProcessor.formatManager.createReaderFor (file);

                                  if (reader != nullptr)
                                  {
                                      auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
                                      audioProcessor.transportSource.setSource (newSource.get (), 0, nullptr, reader->sampleRate);
                                      changeState (TransportState::Stopping);
                                      playButton.setEnabled (true);
                                      playButton.setButtonText ("Play");
                                      audioProcessor.readerSource.reset (newSource.release ());
                                  }
                              }
                          });
}

void PlayerAudioProcessorEditor::playButtonClicked ()
{
    if (state == TransportState::Playing)    
        changeState (TransportState::Pausing);
    
    else      
        changeState (TransportState::Starting);
}

void PlayerAudioProcessorEditor::stopButtonClicked ()
{
    if (state == TransportState::Paused)      
        changeState (TransportState::Stopped);
    
    else 
        changeState (TransportState::Stopping);   
}