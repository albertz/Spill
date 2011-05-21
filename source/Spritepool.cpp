/*
 * Spritepool.cpp
 *
 *  Created on: 19.05.2011
 *      Author: Sebastian
 */

#include "Spritepool.hpp"
#include "Block.hpp"
#include "Logfile.hpp"
#include "consts.hpp"

#include "AttackAnimations/CAA_Explosion1.hpp"
#include "AttackAnimations/CAA_Cloud.hpp"
#include "items/IFlag.hpp"
#include "items/IMoney.hpp"


using namespace std;

CSpritepool::CSpritepool() {
	//load all sprites in Heap!
	CSprite * pSprite;

//Cannonball
//0
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/CannonBall.bmp");
	m_pSprites.push_back(pSprite);
	pSprite = NULL;

//Explosion
//1
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/Explosion1.bmp", 6, 30, 30);
	m_pSprites.push_back(pSprite);
	pSprite = NULL;

//Cloud
//2
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/cloudfinal.bmp", true);
	m_pSprites.push_back(pSprite);
	pSprite = NULL;

//Flag:
//3
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/Flag.bmp", true);
	m_pSprites.push_back(pSprite);
	pSprite = NULL;
//4
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/IconFlag.bmp", true, false);
	m_pSprites.push_back(pSprite);
	pSprite = NULL;

//DummyIcon:
//5
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/EmptyItemSlot.bmp", true, false);
	CGame::setDummyItemIcon(pSprite);
	m_pSprites.push_back(pSprite);
	pSprite = NULL;

//Money
//6
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/IconMoney.bmp", true, false);
	m_pSprites.push_back(pSprite);
	pSprite = NULL;
//7
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/Money.bmp", true);
	m_pSprites.push_back(pSprite);
	pSprite = NULL;

//Worms:
//8
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/christmaswormjustwalk.bmp", 3 , 25, 18);
	//Worm picks it out in itself!
	m_pSprites.push_back(pSprite);
	pSprite = NULL;

//INVI:
//9
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/INVI.bmp", true);
	//Worm picks it out in itself!
	m_pSprites.push_back(pSprite);
	pSprite = NULL;
//10
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/IconINVI.bmp", true, false);
	//Worm picks it out in itself!
	m_pSprites.push_back(pSprite);
	pSprite = NULL;

//Itembar
//10
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/ItemBar_Front.bmp", true, false);
	//Worm picks it out in itself!
	m_pSprites.push_back(pSprite);
	pSprite = NULL;
//10
	pSprite = new CSprite();
	pSprite->Load(_DIRDATA_+"/ItemBar_Back.bmp", true, false);
	//Worm picks it out in itself!
	m_pSprites.push_back(pSprite);
	pSprite = NULL;

//FIXME:^^handle this better not with these defines... its ugly to do it like this... -_-
	//Maybe sprites should get a namemember....

	//Blocksprites: HINT: has to be the last!!
	CBlock::m_pBlockSprites[0] = new CSprite(_DIRDATA_+"/BlockAir.bmp");
	CBlock::m_pBlockSprites[1] = new CSprite(_DIRDATA_+"/BlockNormal.bmp");
	CBlock::m_pBlockSprites[2] = new CSprite(_DIRDATA_+"/BlockShooting.bmp");
	CBlock::m_pBlockSprites[3] = new CSprite(_DIRDATA_+"/BlockJumpboard.bmp");
	CBlock::m_pBlockSprites[4] = new CSprite(_DIRDATA_+"/BlockInvisible.bmp");
	CBlock::m_pBlockSprites[5] = new CSprite(_DIRDATA_+"/BlockCannon.bmp");
	CBlock::m_pBlockSprites[6] = new CSprite(_DIRDATA_+"/BlockCloudGen.bmp");
	for (int i = 0; i < BLOCKAMOUNT; i++) {//push them into the vector!
		m_pSprites.push_back(CBlock::m_pBlockSprites[i]);
	}

	/*
	 * Texture
	 */
	CTexture * pTexture;
	pTexture = new CTexture();
	pTexture->Load(_DIRDATA_+"/lightening.bmp");
	CAA_Cloud::setTextureSprite(pTexture);
	//TODO: pTexture isn't freed at the end!
	pTexture = NULL;

	g_pLogfile->Textout("<br />Successful loaded Sprites!");
}

CSpritepool::~CSpritepool() {
	vector<CSprite*>::iterator it;
	for (it = m_pSprites.begin(); it != m_pSprites.end(); ++it) {
		delete ((*it));
	}
}

CSprite* CSpritepool::at(int i) {
	return m_pSprites.at(i);
}

CSprite* CSpritepool::last() {
	return m_pSprites.back();
}
