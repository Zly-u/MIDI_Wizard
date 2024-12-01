#pragma once

#include <memory>

#include "midi_datatypes.h"
#include "midi_globals.h"

class MidiTrack {
    public:
        template<typename T>
        struct MinMax {

            T Min;
            T Max;

        	[[nodiscard]] T GetDifference() const {
        		if(Min > Max) {
        			return Min - Max;
        		}
        		return Max - Min;
        	}
        };

        struct Note {
            uint64_t start;
            uint64_t length;
            uint8_t  pitch;
            uint8_t  vel;
        };

    public:
        MidiTrack(
            Track* new_midi_track_data,
            const uint8_t track_id
        ) :
            track_id(track_id),
            m_data_midi_track(new_midi_track_data)
        {
        	name           = m_data_midi_track->name;
	        auto& NoteOns  = m_data_midi_track->events[globals::midi::event_names::NoteOn];
            auto& NoteOffs = m_data_midi_track->events[globals::midi::event_names::NoteOff];

            for(size_t index = 0; index < NoteOns.size(); ++index){
                if (NotesTimeRange.Min >= NoteOns[index]->time) {
                    NotesTimeRange.Min = NoteOns[index]->time;
                }

                if (NotesPitchRange.Max <= NoteOns[index]->value1) {
                    NotesPitchRange.Max = NoteOns[index]->value1;
                }
            	if (NotesPitchRange.Min >= NoteOns[index]->value1) {
            		NotesPitchRange.Min = NoteOns[index]->value1;
            	}

                if(NoteOffs.size() <= index) {
                    notes.emplace_back(
                        NoteOns[index]->time,
                        100,
                        (uint8_t)NoteOns[index]->value1,
                        (uint8_t)NoteOns[index]->value2
                    );
                    continue;
                }

                if (NotesTimeRange.Max <= NoteOffs[index]->time) {
                    NotesTimeRange.Max = NoteOffs[index]->time;
                }

                notes.emplace_back(
                    NoteOns[index]->time,
                    NoteOffs[index]->time - NoteOns[index]->time,
                    (uint8_t)NoteOns[index]->value1,
                    (uint8_t)NoteOns[index]->value2
                );
            }
        }

        std::vector<Note>& GetNotes() {
            return notes;
        }

        MinMax<uint64_t> NotesTimeRange{INT_MAX, 0};
        MinMax<uint8_t>  NotesPitchRange{127, 0};


        std::string       name;
        std::vector<Note> notes;
        uint8_t           track_id;


    private:
        Track* m_data_midi_track;

};
