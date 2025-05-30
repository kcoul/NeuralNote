#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

#include "BasicPitch.h"
#include "MidiFileWriter.h"
#include "TimeQuantizeOptions.h"

std::string convertMillis(long long milliseconds) {
    auto duration = std::chrono::milliseconds(milliseconds);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);

    if (hours.count() > 0)
        return std::to_string(hours.count()) + ":" +
               std::to_string(minutes.count()) + ":" +
               std::to_string(seconds.count());
    else
        return std::to_string(minutes.count()) + ":" +
               std::to_string(seconds.count());
}

struct App : juce::JUCEApplication
{
    const juce::String getApplicationName() override
    {
        return "NeuralNoteCLI";
    }

    const juce::String getApplicationVersion() override { return "0.1"; }
    void initialise(const juce::String& inputArgs) override
    {
        juce::StringArray tokens;
        tokens.addTokens (inputArgs, " ", "\"");

        if (tokens.size() != 2)
        {
            std::cerr << "Usage: <wav file> <out dir>"
                      << std::endl;
            exit(1);
        }

        // load audio passed as argument
        std::string wav_file = tokens[0].toStdString();
        juce::File input { wav_file };

        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        std::unique_ptr<juce::FileInputStream> inputStream = input.createInputStream();

        std::unique_ptr<juce::AudioBuffer<float>> buffer = std::make_unique<juce::AudioBuffer<float>>();

        juce::AudioFormatReader* reader = formatManager.createReaderFor (input);

        if (reader != nullptr)
        {
            buffer->setSize(reader->numChannels, (int)reader->lengthInSamples);
            reader->read(buffer.get(),
                         0,
                         (int)reader->lengthInSamples,
                         0,
                         true,
                         true);
        }

        // strip extension to make prefix for output filenames
        std::filesystem::path output_file_prefix = wav_file;
        output_file_prefix.replace_extension();

        // output dir passed as argument
        std::string out_dir = tokens[1].toStdString();

        // Check if the output directory exists, and create it if not
        std::filesystem::path output_dir_path(out_dir);
        if (!std::filesystem::exists(output_dir_path))
        {
            std::cerr << "Directory does not exist: " << out_dir << ". Creating it."
                      << std::endl;
            if (!std::filesystem::create_directories(output_dir_path))
            {
                std::cerr << "Error: Unable to create directory: " << out_dir
                          << std::endl;
                return;
            }
        }
        else if (!std::filesystem::is_directory(output_dir_path))
        {
            std::cerr << "Error: " << out_dir << " exists but is not a directory!"
                      << std::endl;
            return;
        }

        BasicPitch basicPitch;

        float inNoteSensitivity = 0.7;
        float inPitchSensitivity = 0.5;
        float inMinNoteDurationMs = 50;

        basicPitch.setParameters(inNoteSensitivity, inPitchSensitivity, inMinNoteDurationMs);

        auto start = std::chrono::high_resolution_clock::now();
        basicPitch.transcribeToMIDI(buffer->getWritePointer(0),
                                    buffer->getNumSamples());
        auto stop = std::chrono::high_resolution_clock::now();
        auto processingTime = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Audio converted to MIDI in " << convertMillis(processingTime.count()) << "s" << std::endl;

        auto noteEvents = basicPitch.getNoteEvents();

        auto outFileName = output_file_prefix.string() + ".mid";

        auto fullOutputPath = output_dir_path / outFileName;

        juce::File outFile { fullOutputPath.string() };

        MidiFileWriter midiFileWriter;
        TimeQuantizeOptions::TimeQuantizeInfo quantizeInfo;
        double bpm = 120;
        PitchBendModes pitchBendMode = PitchBendModes::NoPitchBend;

        if (!midiFileWriter.writeMidiFile(noteEvents, outFile, quantizeInfo, bpm, pitchBendMode)) {
            std::cout << "Writing MIDI file failed" << std::endl;
        }

        delete(reader);

        quit();
    }

    void shutdown() override
    {

    }
};

START_JUCE_APPLICATION(App)