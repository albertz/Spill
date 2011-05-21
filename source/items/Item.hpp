/*
 * Item.hpp
 *
 *  Created on: 19.05.2011
 *      Author: Sebastian
 */

#ifndef ITEM_HPP_
#define ITEM_HPP_

#include "../Sprite.hpp"
#include "../PhysicalObject.hpp"
#include <string>

//To be able to declare prototype of setowner:
class CWorm;

class CItem : public CPhysicalObject {
private:
	CSprite * m_pIcon;
	CWorm * m_pOwner;
	bool m_bIsAlive;
	std::string m_Name;

public:
	void setName(std::string name) {
		m_Name = name;
	}
	std::string getName() {
		return m_Name;
	}

	virtual ~CItem() {};

	void init();
	virtual void use() {};
	virtual void update() {};
	virtual void render() {};
	virtual void quit() {};



	virtual void onSetOwner(CWorm * pOwner) {};
	void setOwner(CWorm * pOwner) {
		m_pOwner = pOwner;
		onSetOwner(pOwner);
	};

	CWorm * getOwner() {
		return m_pOwner;
	};

//FIXME: zauberstab zum unsichtbar machen!

	void renderIcon(int x, int y);
	void setIcon(CSprite * pIcon);

	void setIsAlive(bool alive) {
		m_bIsAlive = alive;
	}

	bool isAlive() {
		return m_bIsAlive;
	}
};


#endif /* ITEM_HPP_ */
