/*
 * Menu.cpp
 *
 *  Created on: 10.05.2011
 *      Author: Sebastian
 */

#include "Menu.hpp"

#include "../Framework.hpp"
#include "MenuSystem.hpp" //MBE Kreisincludierung!?!

using namespace std;

void CMenu::init( std::string heading) {
	m_selectedItem = m_Items.begin();
	m_Heading = heading;
	m_bKeyLock = false;
}

void CMenu::addItem(string name, int messageID, EntryType entryType) {
	S_MenuItem item;
	item.m_MessageID = messageID;
	item.m_Name = name;
	item.m_EntryType = entryType;
	item.m_Text = "";
	m_Items.push_back(item);
	m_selectedItem = m_Items.begin();
}

void CMenu::render() {
	//Draw Heading: MBE: Viewport????!?!!
	g_pFramework->TextOut(m_Heading, 50, 10, 0);
	//draw items!
	vector<S_MenuItem>::iterator it;
	int y = 50 + 14;
	for (it = m_Items.begin(); it !=  m_Items.end(); ++it) {
		y += 18;
		g_pFramework->TextOut(it->m_Name,  70, y, 0);
		g_pFramework->TextOut(it->m_Text, 500, y, 0);
		if (it == m_selectedItem) { //draw line under it!
			g_pFramework->drawLine(CVec(0, y+17), CVec(800,y+17), 0, 255, 0, false);
			//MBE draws line just on viewport!
		}
	}
}

void CMenu::quit() {
	m_Items.clear();
}

void CMenu::update() {
	if (m_selectedItem->m_EntryType == CMenu::TEXTBOX) {
		//put incoming letterkeys in m_text!
		if (g_pFramework->KeyDown(SDLK_BACKSPACE)) //delete last letter FIXME!
			m_selectedItem->m_Text.at(m_selectedItem->m_Text.length()-1);
		SDL_Event event;
		if (SDL_PollEvent(&event)) { //TODO hoffe das geht so das wir das jez in einer gameschleife im prinzip 2x tun!
			if (event.type == SDL_KEYDOWN) {

				char key = event.key.keysym.sym;
				if ((97 <= key) && (key <= 122)) { //is letter key!
					m_selectedItem->m_Text.append(&key);
				}



			}
		} //if


	}


	if ( (!m_bKeyLock) && (g_pFramework->KeyDown(SDLK_DOWN)) ) {
		m_selectedItem ++;
		m_bKeyLock = true;
	}
	if ( (!m_bKeyLock) && (g_pFramework->KeyDown(SDLK_UP)) ) {
		m_selectedItem --;
		m_bKeyLock = true;
	}

	//MBE operator wirklich richtig definiert????
	if (m_selectedItem < m_Items.begin()) {
		m_selectedItem = m_Items.end();
		m_selectedItem --;
	}

	if (m_selectedItem >= m_Items.end())
		m_selectedItem = m_Items.begin();

	if ( (!m_bKeyLock) && (g_pFramework->KeyDown(SDLK_RETURN)) ) {
		//Change Checkboxstate on enter
		if ( (m_selectedItem->m_EntryType == CMenu::CHECKBOX)){
			if (m_selectedItem->m_Text == "")
				m_selectedItem->m_Text = "1";
			else
				m_selectedItem->m_Text = "0";
		}

		//do process!
		g_pMenuSystem->ProcessSelection(m_selectedItem->m_MessageID);
		m_bKeyLock = true;
	}

	if ( (m_bKeyLock)
				&& (g_pFramework->KeyDown(SDLK_DOWN) 	== false )
				&& (g_pFramework->KeyDown(SDLK_UP) 		== false )
				&& (g_pFramework->KeyDown(SDLK_RETURN) 	== false ) )
			m_bKeyLock = false;
}

string CMenu::getItemText() {
	return m_selectedItem->m_Text;
}
