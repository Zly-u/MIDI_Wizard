#pragma once

#include <map>
#include <string>

namespace globals {
    namespace event_names {
        inline std::string NoteOff = "Note OFF";
        inline std::string NoteOn = "Note ON";
        inline std::string NoteAftertouch = "Note Aftertouch";
        inline std::string Controller = "Controller";
        inline std::string ProgramChange = "Program Change";
        inline std::string ChannelAftertouch = "Channel Aftertouch";
        inline std::string PitchBend = "Pitch Bend";
    }


    namespace midi{
	    const std::map<uint8_t, std::string> meta_event_names = {
		    {0x00, "Sequence Number"},
		    {0x01, "Text"},
		    {0x02, "Copyright Notice"},
		    {0x03, "Track Name"},
		    {0x04, "Instrument Name"},
		    {0x05, "Lyrics"},
		    {0x06, "Marker"},
		    {0x07, "Cue Point"},
		    {0x20, "MIDI Channel Prefix"},
		    {0x2F, "END OF TRACK"},
		    {0x51, "Set Tempo"},
		    {0x54, "SMPTE Offset"},
		    {0x58, "Time Signature"},
		    {0x59, "Key Signature"},
		    {0x7F, "Sequencer Specific"},
	    };

	    const std::map<uint8_t, std::string> event_names_by_code = {
		    {0x80, event_names::NoteOff},
		    {0x90, event_names::NoteOn},
		    {0xA0, event_names::NoteAftertouch},
		    {0xB0, event_names::Controller},
		    {0xC0, event_names::ProgramChange},
		    {0xD0, event_names::ChannelAftertouch},
		    {0xE0, event_names::PitchBend},
	    };

	    constexpr char note_names[12][3] = {
		    "C ",
		    "C#",
		    "D ",
		    "D#",
		    "E ",
		    "F ",
		    "F#",
		    "G ",
		    "G#",
		    "A ",
		    "A#",
		    "B ",
	    };
    }
}