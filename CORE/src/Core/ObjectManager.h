#pragma once

#include <memory>
#include <vector>

#include "MidiTrack.h"
#include "Object.h"

class ObjectManager {
public:
	ObjectManager(const ObjectManager&) = delete; 
	ObjectManager(const ObjectManager&&) = delete; 

	~ObjectManager() {}

	
public:
	using SharedObj = std::shared_ptr<Object>;
	using ObjectsVector = std::vector<SharedObj>;

	
public:
	static ObjectManager& Get() {
		static ObjectManager s_instance;
		return s_instance;
	}

	
	template<class T, typename ...Args>
	static std::shared_ptr<T> Create(Args... p) {
		if(!std::is_base_of_v<Object, T>) {
			assert(false && "The passed class is not deriveted from Object");
		}

		std::shared_ptr<T> new_object = std::make_shared<T>(p...);

		ObjectsVector* objects;
		if(std::is_base_of_v<MidiTrack, T>) {
			objects = &GetTracks();
		} else {
			objects = &GetObjects();
		}

		bool bHasEmplaced = false;
		for(auto It = objects->begin(); It != objects->end(); ++It) {
			std::weak_ptr obj = *It;
			if(!obj.expired()) { continue; }

			*It = new_object;
			bHasEmplaced = true;
			break;
		}
		if(!bHasEmplaced) {
			objects->emplace_back(new_object);
		}

		return new_object;
	}

	
	static void ClearTracks() { Get().ClearTracks_Impl(); }
	static void Update(const float dt) { Get().Update_Impl(dt); }
	static void Draw() { Get().Draw_Impl(); }

	void ClearTracks_Impl();
	void Update_Impl(const float dt);
	void Draw_Impl();

	static ObjectsVector& GetObjects() { return Get().m_objects; }
	static ObjectsVector& GetTracks()  { return Get().m_tracks; }

	
private:
	ObjectManager() = default;

	
private:
	ObjectsVector m_objects;
	ObjectsVector m_tracks;
};
