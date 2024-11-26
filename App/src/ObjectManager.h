#pragma once

#include <memory>
#include <vector>

#include "Object.h"
#include "Core/helpers.h"


class UI_Element_midi_track;

class ObjectManager : Singleton<ObjectManager> {
public:
	// using SharedObj = std::shared_ptr<Object>;
	using SharedObj = Object;
	using ObjectsVector = std::vector<SharedObj>;
	
public:
	template<class T, typename ...Args>
	static SharedObj& Create(Args... p) {
		if(!std::is_base_of_v<Object, T>) {
			assert(false && "The passed class is not deriveted from Object");
		}

		ObjectsVector* objects;
		if(std::is_base_of_v<UI_Element_midi_track, T>) {
			objects = &GetTracks();
		} else {
			objects = &GetObjects();
		}

		return objects->emplace_back(T(p...));
	}


	static void Update(const float dt)	{ Get().Update_Impl(dt); }
	static void Draw()					{ Get().Draw_Impl(); }


	void Update_Impl(float dt);
	void Draw_Impl();
    void ClearTracks_Impl();
	void ClearObjects_Impl();

	static void ClearTracks()           { Get().ClearTracks_Impl(); }
	static void ClearObjects()           { Get().ClearObjects_Impl(); }
	static ObjectsVector& GetObjects()  { return Get().m_objects; }
	static ObjectsVector& GetTracks()   { return Get().m_tracks; }

	
private:
	ObjectsVector m_objects;
	ObjectsVector m_tracks;
};
