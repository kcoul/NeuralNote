//
// Created by Damien Ronssin on 11.03.23.
//

#ifndef VisualizationPanel_h
#define VisualizationPanel_h

#include <JuceHeader.h>

#include "CombinedAudioMidiRegion.h"
#include "Keyboard.h"
#include "MidiFileDrag.h"
#include "PluginProcessor.h"
#include "VisualizationPanel.h"

class VisualizationPanel : public juce::Component
{
public:
    explicit VisualizationPanel(NeuralNoteAudioProcessor* processor);

    void resized() override;

    void paint(Graphics& g) override;

    void clear();

    void repaintPianoRoll();

    void setMidiFileDragComponentVisible();

    void mouseEnter(const juce::MouseEvent& event) override;

    void mouseExit(const juce::MouseEvent& event) override;

    Viewport& getAudioMidiViewport();

    CombinedAudioMidiRegion& getCombinedAudioMidiRegion();

    static constexpr int KEYBOARD_WIDTH = 50;

private:
    NeuralNoteAudioProcessor* mProcessor;
    Keyboard mKeyboard;
    juce::Viewport mAudioMidiViewport;
    CombinedAudioMidiRegion mCombinedAudioMidiRegion;
    MidiFileDrag mMidiFileDrag;

    juce::Slider mAudioGainSlider;
    std::unique_ptr<juce::SliderParameterAttachment> mAudioGainSliderAttachment;

    juce::Slider mMidiGainSlider;
    std::unique_ptr<juce::SliderParameterAttachment> mMidiGainSliderAttachment;

    juce::Rectangle<int> mAudioRegionBounds;
    juce::Rectangle<int> mPianoRollBounds;

    std::unique_ptr<juce::TextEditor> mFileTempo;
};

#endif // VisualizationPanel_h
