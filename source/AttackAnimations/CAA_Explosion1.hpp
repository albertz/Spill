/*
 * CAA_Explosion1.hpp
 *
 *  Created on: 09.05.2011
 *      Author: Sebastian
 */

#ifndef CAA_EXPLOSION1_HPP_
#define CAA_EXPLOSION1_HPP_

#include "AttackAnimation.hpp"
#include "../Sprite.hpp"
#include "../Vec.hpp"

class CAA_Explosion1: public CAttackAnimation {
private:
	static CSprite * m_pSprite;
	float m_fAnimPhase;
	CVec m_where;

public:
	static void setSprite(CSprite * pSprite) {
		CAA_Explosion1::m_pSprite = pSprite;
	}

	virtual void init(CVec &where);
	virtual void render();
	virtual void update();
	virtual void quit() {};

	CAA_Explosion1() {};
	virtual ~CAA_Explosion1() {};
};

#endif /* CAA_EXPLOSION1_HPP_ */
