#pragma once

#include <cstdint>
#include <fstream>
#include <mutex>

#include "midi_datatypes.h"
#include "midi_globals.h"


namespace std {
	class stop_token;
}

//////////////////////////////////////////////////////

namespace MidiParserConvertors{
	using namespace globals::midi;

	/*
		http://www.ccarh.org/courses/253/handout/vlv/
		Definition of Variable Length Values

		A MIDI file Variable Length Value is stored in bytes.
		Each byte has two parts: 7 bits of data and 1 continuation bit.
		The highest-order bit is set to 1 if there is another byte of the number to follow.
		The highest-order bit is set to 0 if this byte is the last byte in the VLV.

		To recreate a number represented by a VLV, first you remove the continuation bit and
		then concatenate the leftover bits into a single number.

		To generate a VLV from a given number, break the number up into 7 bit units and
		then apply the correct continuation bit to each byte.

		In theory, you could have a very long VLV number which was quite large;
		however, in the standard MIDI file specification, the maximum length of
		a VLV value is 5 bytes, and the number it represents can not be larger than 4 bytes.
	*/
	inline uint64_t parse_vlv(std::ifstream& file) {
		uint64_t value = 0;
		uint8_t  byte  = 0;
		unsigned char fetched_byte = 0;

		while(true) {
			byte++;
			file.read(reinterpret_cast<char*>(&fetched_byte), sizeof(char));

			// Append the byte into our variable
			value <<= 7;
			value |= fetched_byte & 0x7f;

			if(!(fetched_byte & 0x80) || byte >= 4) {// Potentially can remove byte restriction for longer Delta-Times
				break;
			}
		}

		return value;
	}

	template <typename Type>
	void flip_bytes_inplace(Type& origValue) {
		constexpr size_t TypeSize = sizeof(Type);

		char* OrigBytes = reinterpret_cast<char*>(&origValue);

		for(size_t i = 0; i < TypeSize / 2; ++i) {
			const char lastByte = OrigBytes[(TypeSize - 1) - i];
			OrigBytes[(TypeSize - 1) - i] = OrigBytes[i];
			OrigBytes[i] = lastByte;
		}
	}

	// TODO: Time Signature is needed for `division`
	inline long double convert_dt_to_ms(const uint64_t current_tick, const uint64_t tempo, const uint64_t division) {
		return (double)current_tick * ((double)tempo / (double)division) * 1000.0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<int32_t>
	struct Convert {};

	//////// Sys Ex Events ////////

	template<>
	struct Convert<0xF0> {
		// NOTE: we just ignore the data for now.
		static void decode(std::ifstream& midi_file, SysExEvent* new_sysex_event) {
			// SysExEvent new_sys_ex_event;
			// new_sys_ex_event.type = event;
			// new_sys_ex_event.name = "SysEx";
			// new_sys_ex_event.time = absolute_time;
			int64_t sysex_size = 0;
			midi_file.read(reinterpret_cast<char*>(&sysex_size), sizeof(char));
			midi_file.ignore(sysex_size - 1);	// Skip packet
			midi_file.ignore();					// Skip ending byte.
		}
		static void encode() {

		}
	};

	//////// Meta Events ////////

	template<>
		struct Convert<meta_event_codes::SequenceNumber> {
		static void decode(std::ifstream& midi_file, MetaEvent* new_meta_event) {
			uint8_t MSB = 0; // [0, 255]
			uint8_t LSB = 0; // [0, 255]

			midi_file.ignore(); // Ignoring size byte
			midi_file.read(reinterpret_cast<char*>(&MSB), sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&LSB), sizeof(char));
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<meta_event_codes::Text> {
		/* So far is used for:
			meta_event_codes::Text
			meta_event_codes::CopyrightNotice
			meta_event_codes::TrackName
			meta_event_codes::InstrumentName
			meta_event_codes::Lyrics
			meta_event_codes::Marker
			meta_event_codes::CuePoint
			meta_event_codes::SequencerSpecific
		*/
		static void decode(std::ifstream& midi_file, MetaEvent* new_meta_event) {
			const uint64_t  meta_event_size = parse_vlv(midi_file);
			char*           text            = new char[meta_event_size+1];

			midi_file.read(text, sizeof(char) * meta_event_size);
			text[meta_event_size] = '\0';

			new_meta_event->text = text;

			delete[] text;
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<meta_event_codes::MIDIChannelPrefix> {
		static void decode(std::ifstream& midi_file, MetaEvent* new_meta_event) {
			uint64_t& channel = new_meta_event->value1; // [0, 15]

			midi_file.ignore(); // Ignoring size byte
			midi_file.read(reinterpret_cast<char*>(&channel), sizeof(char));
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<meta_event_codes::SetTempo> {
		// MICROSECONDS_PER_MINUTE = 60000000 BPM = MICROSECONDS_PER_MINUTE / MPQNMPQN = MICROSECONDS_PER_MINUTE / BPM
		// If not provided, the tempo should be set to 120 BPM.

		static void decode(std::ifstream& midi_file, MetaEvent* new_meta_event) {
			uint64_t& tempo = new_meta_event->value1; // 0-8355711

			midi_file.ignore(); // Ignoring size byte
			midi_file.read(reinterpret_cast<char*>(&tempo), sizeof(char) * 3);
			flip_bytes_inplace(tempo);
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<meta_event_codes::SMPTEOffset> {
		static void decode(std::ifstream& midi_file, MetaEvent* new_meta_event) {
			uint64_t& hour   = new_meta_event->value1; // [0-23]
			uint64_t& min    = new_meta_event->value2; // [0-59]
			uint64_t& sec    = new_meta_event->value3; // [0-59]
			uint64_t& fr     = new_meta_event->value4; // [0-30]
			uint64_t& sub_fr = new_meta_event->value5; // [0-99]

			midi_file.ignore(); // Ignoring size value
			midi_file.read(reinterpret_cast<char*>(&hour),	sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&min),	sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&sec),	sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&fr),	sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&sub_fr),sizeof(char));
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<meta_event_codes::TimeSignature> {
		static void decode(std::ifstream& midi_file, MetaEvent* new_meta_event) {
			uint64_t& number      = new_meta_event->value1; // [0-255]
			uint64_t& denominator = new_meta_event->value2; // [0-255]
			uint64_t& metro       = new_meta_event->value3; // [0-255]
			uint64_t& _32nds      = new_meta_event->value4; // [1-255]

			midi_file.ignore(); // Ignoring size byte
			midi_file.read(reinterpret_cast<char*>(&number),		sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&denominator),	sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&metro),			sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&_32nds),		sizeof(char));
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<meta_event_codes::KeySignature> {
		static void decode(std::ifstream& midi_file, MetaEvent* new_meta_event) {
			int8_t  key;	//[-7, 7]
			bool	scale;	//major/minor

			midi_file.ignore(); // Ignoring size byte
			midi_file.read(reinterpret_cast<char*>(&key),	sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&scale),	sizeof(char));

			new_meta_event->value1 = key + 7;
			new_meta_event->value2 = scale;
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<meta_event_codes::EndOfTrack> {
		static void decode(std::ifstream& midi_file, MetaEvent* new_meta_event) {
			// Skip last 0x0 of the ending pattern
			// pattern: 0xFF 0x2F 0x00
			midi_file.ignore(); // Ignoring size byte
		}
		static void encode() {

		}
	};

	///////////////////////////

	// Regular Events
	template<>
	struct Convert<event_codes::NoteOff> {
		static void decode(std::ifstream& midi_file, MIDI_Event* new_event) {
			uint8_t note;		// [0-127]
			uint8_t vel;		// [0-127]

			midi_file.read(reinterpret_cast<char*>(&note),	sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&vel),	sizeof(char));

			new_event->value1 = note;
			new_event->value2 = vel;
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<event_codes::NoteOn> {
		static void decode(std::ifstream& midi_file, MIDI_Event* new_event) {
			uint8_t note;		// [0-127]
			uint8_t vel;		// [0-127]

			midi_file.read(reinterpret_cast<char*>(&note),	sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&vel),	sizeof(char));

			new_event->value1 = note;
			new_event->value2 = vel;
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<event_codes::NoteAftertouch> {
		static void decode(std::ifstream& midi_file, MIDI_Event* new_event) {
			uint8_t note;		// [0-127]
			uint8_t amount;		// [0-127]

			midi_file.read(reinterpret_cast<char*>(&note),		sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&amount),	sizeof(char));

			new_event->value1 = note;
			new_event->value2 = amount;
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<event_codes::Controller> {
		/*	CONTROLLER TYPES:
			0		(0x00) 			Bank Select
			1		(0x01) 			Modulation
			2		(0x02) 			Breath Controller
			4		(0x04) 			Foot Controller
			5		(0x05) 			Portamento Time
			6		(0x06) 			Data Entry (MSB)
			7		(0x07) 			Main Volume
			8		(0x08) 			Balance
			10		(0x0A) 			Pan
			11		(0x0B) 			Expression Controller
			12		(0x0C) 			Effect Control 1
			13		(0x0D) 			Effect Control 2
			16-19	(0x10-0x13) 	General-Purpose Controllers 1-4
			32-63	(0x20-0x3F) 	LSB for controllers 0-31
			64		(0x40) 			Damper pedal (sustain)
			65		(0x41) 			Portamento
			66		(0x42) 			Sostenuto
			67		(0x43) 			Soft Pedal
			68		(0x44) 			Legato Footswitch
			69		(0x45) 			Hold 2
			70		(0x46) 			Sound Controller 1 (default: Timber Variation)
			71		(0x47) 			Sound Controller 2 (default: Timber/Harmonic Content)
			72		(0x48) 			Sound Controller 3 (default: Release Time)
			73		(0x49) 			Sound Controller 4 (default: Attack Time)
			74-79	(0x4A-0x4F) 	Sound Controller 5-10
			80-83	(0x50-0x53) 	General-Purpose Controllers 5-8
			84		(0x54) 			Portamento Control
			91		(0x5B) 			Effects 1 Depth (formerly External Effects Depth)
			92		(0x5C) 			Effects 2 Depth (formerly Tremolo Depth)
			93		(0x5D) 			Effects 3 Depth (formerly Chorus Depth)
			94		(0x5E) 			Effects 4 Depth (formerly Celeste Detune)
			95		(0x5F) 			Effects 5 Depth (formerly Phaser Depth)
			96		(0x60) 			Data Increment
			97		(0x61) 			Data Decrement
			98		(0x62) 			Non-Registered Parameter Number (LSB)
			99		(0x63) 			Non-Registered Parameter Number (MSB)
			100		(0x64) 			Registered Parameter Number (LSB)
			101		(0x65) 			Registered Parameter Number (MSB)
			121-127	(0x79-0x7F) 	Mode Messages
		*/
		static void decode(std::ifstream& midi_file, MIDI_Event* new_event) {
			uint8_t controller_type;	// [0-127]
			uint8_t value;				// [0-127]

			midi_file.read(reinterpret_cast<char*>(&controller_type),	sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&value),				sizeof(char));

			new_event->value1 = controller_type;
			new_event->value2 = value;
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<event_codes::ProgramChange> {
		static void decode(std::ifstream& midi_file, MIDI_Event* new_event) {
			uint8_t program_number;	// [0-127]

			midi_file.read(reinterpret_cast<char*>(&program_number), sizeof(char));

			new_event->value1 = program_number;
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<event_codes::ChannelAftertouch> {
		static void decode(std::ifstream& midi_file, MIDI_Event* new_event) {
			uint8_t amount;		// [0-127]

			midi_file.read(reinterpret_cast<char*>(&amount), sizeof(char));

			new_event->value1 = amount;
		}
		static void encode() {

		}
	};

	template<>
	struct Convert<event_codes::PitchBend> {
		static void decode(std::ifstream& midi_file, MIDI_Event* new_event) {
			uint8_t LSB;		// [0-127]
			uint8_t MSB;		// [0-127]

			midi_file.read(reinterpret_cast<char*>(&LSB), sizeof(char));
			midi_file.read(reinterpret_cast<char*>(&MSB), sizeof(char));

			new_event->value1 = LSB;
			new_event->value2 = MSB;
		}
		static void encode() {

		}
	};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MidiParser {
public:
	static MIDI_ParsedData midi_header;
	static midi parsed_midi;
	
	static void worker_TrackRead(const std::stop_token& stop_token, const wchar_t* file_path, const uint16_t track_index);
	static bool Read(wchar_t* file_path);

private:
	static std::mutex g_track_write_mutex;
};
