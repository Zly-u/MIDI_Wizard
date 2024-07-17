#include "ObjectManager.h"

#include "UI_Element_midi_track.h"

class UI_Element_midi_note;

void ObjectManager::ClearTracks_Impl() {
	GetTracks().clear();
}

void ObjectManager::Update_Impl(float dt) {
	for(std::weak_ptr<Object> track : m_tracks) {
		if(track.expired()) { continue; }
		track.lock()->Update(dt);
	}
	
	for(std::weak_ptr<Object> obj : m_objects) {
		if(obj.expired()) { continue; }
		obj.lock()->Update(dt);
	}
}

void ObjectManager::Draw_Impl() {
	for(std::weak_ptr<Object> track : m_tracks) {
		if(track.expired()) { continue; }
		track.lock()->Draw();
	}

	for(std::weak_ptr<Object> obj : m_objects) {
		if(obj.expired()) { continue; }
		obj.lock()->Draw();
	}
}