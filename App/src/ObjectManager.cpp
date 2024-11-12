#include <vector>

#include "ObjectManager.h"

#if MTR_ENABLED
	#include "minitrace.h"
#endif

class UI_Element_midi_note;


void ObjectManager::ClearTracks_Impl() {
	GetTracks().clear();
}


void ObjectManager::Update_Impl(const float dt) {
	for(auto& track : m_tracks) {
		track.Update(dt);
	}
	
	for(auto& obj : m_objects) {
		obj.Update(dt);
	}
}


void ObjectManager::Draw_Impl() {
	MTR_SCOPE("ObjectManager", "Draw_Impl");

	for(auto& track : m_tracks) {
		track.Draw();
	}

	for(auto& obj : m_objects) {
		obj.Draw();
	}
}