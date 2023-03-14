//
// Created by Damien Ronssin on 13.03.23.
//

#ifndef NoteOptions_h
#define NoteOptions_h

#include <JuceHeader.h>
#include "Notes.h"

class NoteOptions
{
public:
    enum RootNote
    {
        A = 0,
        A_sharp,
        B,
        C,
        C_sharp,
        D,
        D_sharp,
        E,
        F_sharp,
        F,
        G_sharp,
        G,
        TotalNumRootNotes
    };

    const juce::StringArray RootNotesSharpStr {
        "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

    const juce::StringArray RootNotesFlatStr {
        "A", "Bb", "B", "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab"};

    enum ScaleType
    {
        Chromatic = 0,
        Major,
        Minor,
        TotalNumScaleTypes
    };

    const juce::StringArray ScaleTypesStr {"Chromatic", "Major", "Minor"};

    enum SnapMode
    {
        Adjust = 0,
        Remove
    };

    const juce::StringArray SnapModesStr {"Adjust", "Remove"};

    void setParameters(RootNote inRootNote,
                       ScaleType inScaleType,
                       SnapMode inSnapMode,
                       int inMinMidiNote,
                       int inMaxMidiNote);

    std::vector<Notes::Event> processKey(const std::vector<Notes::Event>& inNoteEvents);

private:
    RootNote mRootNote = C;
    ScaleType mScaleType = Chromatic;
    SnapMode mSnapMode = Remove;
    int mMinMidiNote = MIN_MIDI_NOTE;
    int mMaxMidiNote = MAX_MIDI_NOTE;

    static std::array<int, 7> _createKeyArray(RootNote inRootNote, ScaleType inScaleType);

    static bool _isInKey(int inMidiNote, const std::array<int, 7>& inKeyArray);

    static int _getClosestMidiNoteInKey(int inMidiNote,
                                        const std::array<int, 7>& inKeyArray,
                                        bool inAdjustUp);

    static int _rootNoteToNoteIdx(RootNote inRootNote);

    /* Returns the note index (between 0 and 11 included, C to B) given midi note number */
    static int _midiToNoteIndex(int inMidiNote);

    static constexpr std::array<int, 7> MAJOR_SCALE_INTERVALS = {0, 2, 4, 5, 7, 9, 11};
    static constexpr std::array<int, 7> MINOR_SCALE_INTERVALS = {0, 2, 3, 5, 7, 8, 10};
};

#endif // NoteOptions_h