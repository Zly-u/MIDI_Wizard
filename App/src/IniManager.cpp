#include "IniManager.h"

// ini::IniFile IniManager::file;


std::string IniManager::fileName = "MIDI_Wizard.ini";
ini::IniFile IniManager::file;

ini::IniFile& IniManager::LoadFile_Impl() {
	file.load(fileName);

	return file;
}

void IniManager::SafeFile_Impl() {
	file.save(fileName);
}
