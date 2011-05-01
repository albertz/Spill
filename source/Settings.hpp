/*
 * Settings.hpp
 *
 *  Created on: 27.04.2011
 *      Author: Sebastian
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <string>
#include <SDL.h>

#include "Singleton.hpp"

struct S_WormSettings {
	SDLKey KeyJump;
	SDLKey KeyLeft;
	SDLKey KeyRight;
	SDLKey KeyMine;
	SDLKey KeySelectBlockType;
	SDLKey KeyBuild;

	std::string name;
};


#define g_pSettings CSettings::get()
class CSettings : public TSingleton<CSettings>{
public:
	CSettings();
	virtual ~CSettings();

	S_WormSettings WormSet[2];

	void SaveToFile	(std::string sFilename);
	void LoadFromFile(std::string sFilename);

};

#endif /* SETTINGS_HPP_ */