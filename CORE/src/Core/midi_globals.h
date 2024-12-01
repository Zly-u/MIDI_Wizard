#pragma once

#include <string>
#include <unordered_map>

namespace globals {
	namespace midi {
	    namespace event_names {
	        inline constexpr char NoteOff[]           = "Note OFF";
	        inline constexpr char NoteOn[]            = "Note ON";
	        inline constexpr char NoteAftertouch[]    = "Note Aftertouch";
	        inline constexpr char Controller[]        = "Controller";
	        inline constexpr char ProgramChange[]     = "Program Change";
	        inline constexpr char ChannelAftertouch[] = "Channel Aftertouch";
	        inline constexpr char PitchBend[]         = "Pitch Bend";
	    }

		namespace event_codes {
			inline constexpr uint8_t NoteOff           = 0x80;
			inline constexpr uint8_t NoteOn            = 0x90;
			inline constexpr uint8_t NoteAftertouch    = 0xA0;
			inline constexpr uint8_t Controller        = 0xB0;
			inline constexpr uint8_t ProgramChange     = 0xC0;
    		inline constexpr uint8_t ChannelAftertouch = 0xD0;
    		inline constexpr uint8_t PitchBend         = 0xE0;
	    }

		namespace meta_event_codes {
    		inline constexpr int32_t SequenceNumber    = 0x00;
		    inline constexpr int32_t Text              = 0x01;
		    inline constexpr int32_t CopyrightNotice   = 0x02;
		    inline constexpr int32_t TrackName         = 0x03;
		    inline constexpr int32_t InstrumentName    = 0x04;
		    inline constexpr int32_t Lyrics            = 0x05;
		    inline constexpr int32_t Marker            = 0x06;
		    inline constexpr int32_t CuePoint          = 0x07;
		    inline constexpr int32_t SequencerSpecific = 0x7F;
		    inline constexpr int32_t MIDIChannelPrefix = 0x20;
		    inline constexpr int32_t SetTempo          = 0x51;
		    inline constexpr int32_t SMPTEOffset       = 0x54;
		    inline constexpr int32_t TimeSignature     = 0x58;
		    inline constexpr int32_t KeySignature      = 0x59;
		    inline constexpr int32_t EndOfTrack        = 0x2F;
	    }


		inline std::unordered_map<uint8_t, std::string> meta_event_names = {
		    {meta_event_codes::SequenceNumber,		"Sequence Number"},
		    {meta_event_codes::Text,				"Text"},
		    {meta_event_codes::CopyrightNotice,		"Copyright Notice"},
		    {meta_event_codes::TrackName,			"Track Name"},
		    {meta_event_codes::InstrumentName,		"Instrument Name"},
		    {meta_event_codes::Lyrics,				"Lyrics"},
		    {meta_event_codes::Marker,				"Marker"},
		    {meta_event_codes::CuePoint,			"Cue Point"},
		    {meta_event_codes::MIDIChannelPrefix,	"MIDI Channel Prefix"},
		    {meta_event_codes::EndOfTrack,			"END OF TRACK"},
		    {meta_event_codes::SetTempo,			"Set Tempo"},
		    {meta_event_codes::SMPTEOffset,			"SMPTE Offset"},
		    {meta_event_codes::TimeSignature,		"Time Signature"},
		    {meta_event_codes::KeySignature,		"Key Signature"},
		    {meta_event_codes::SequencerSpecific,	"Sequencer Specific"},
	    };

	    inline std::unordered_map<uint8_t, std::string> event_names_by_code = {
		    {event_codes::NoteOff,			event_names::NoteOff},
		    {event_codes::NoteOn,			event_names::NoteOn},
		    {event_codes::NoteAftertouch,	event_names::NoteAftertouch},
		    {event_codes::Controller,		event_names::Controller},
		    {event_codes::ProgramChange,	event_names::ProgramChange},
		    {event_codes::ChannelAftertouch,event_names::ChannelAftertouch},
		    {event_codes::PitchBend,		event_names::PitchBend},
	    };

	    inline std::unordered_map<std::string, uint8_t> event_codes_by_name = {
    		{event_names::NoteOff,			event_codes::NoteOff},
		    {event_names::NoteOn,			event_codes::NoteOn},
		    {event_names::NoteAftertouch,	event_codes::NoteAftertouch},
		    {event_names::Controller,		event_codes::Controller},
		    {event_names::ProgramChange,	event_codes::ProgramChange},
		    {event_names::ChannelAftertouch,event_codes::ChannelAftertouch},
		    {event_names::PitchBend,		event_codes::PitchBend},
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
