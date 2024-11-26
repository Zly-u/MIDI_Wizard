#include <vector>

#include "ObjectManager.h"

#include "minitrace.h"

class UI_Element_midi_note;


void ObjectManager::ClearTracks_Impl() {
	GetTracks().clear();
}
void ObjectManager::ClearObjects_Impl() {
	GetObjects().clear();
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
	// MTR_SCOPE("ObjectManager", "Draw_Impl");

	for(auto& track : m_tracks) {
		track.Draw();
	}

	for(auto& obj : m_objects) {
		obj.Draw();
	}
}