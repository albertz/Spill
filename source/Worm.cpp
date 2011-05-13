#include "Worm.hpp"
#include <map>
#include "Framework.hpp"

#include "AttackAnimations/AttackAnimation.hpp"
#include "AttackAnimations/CAA_Laser.hpp"
#include "AttackAnimations/CAA_CannonBall.hpp"
#include "AttackAnimations/CAA_Cloud.hpp"
#include "Collision.hpp"

using namespace std;

CWorm::CWorm(CGame *pGame) {
	m_pWormSprite = NULL;
	m_pGame = pGame;
	m_pSettings = NULL;
}

void CWorm::init(int WormID) {
	init(WormID, WormID, 0, 0, WC_RED);
}

void CWorm::init(int WormID, float X, float Y) {
	init(WormID, WormID, X, Y, WC_RED);
}

void CWorm::init(int WormID, WORMCOLORS WC){
	init(WormID, WormID, 0, 0, WC);
}

void CWorm::init(int WormID, int TeamID, float X, float Y, WORMCOLORS WC) {
	m_WormID = WormID;
	m_TeamID = TeamID;
	setCanJump(false);
	m_Color = WC;
	m_Money = 140;//ForDebugReasons MBE
	m_Points = 0;
	m_Energy = MAXENERGY;
	m_fLastActionTime = 0.0f;

	setCanMove(true);
	
	m_bJumpKeyLock = false;
	m_bBuildKeyLock = false;

	m_bNextBTypeKeyLock = false;
	m_selectedBType = CBlock::NORMAL;

	m_bOrientation = ORIGHT;
	
	m_pWormSprite = new CSprite();
	//m_pWormSprite->Load(_DIRDATA_+"/christmaswormjustwalk.bmp", 21, 32, 17);
	//New animation: //braucht auch anpassung in Animate!
	m_pWormSprite->Load(_DIRDATA_+"/christmaswormjustwalk.bmp", 3 , 25, 18);
	m_fAnimphase = 0.0f;
	m_isWalking = false;
	setIsSolid(false);

	FloatRect FR;
	FR = m_pWormSprite->GetRect();
	FR.x += 100.0f * WormID;
	setRect(FR);
	setDir(CVec(0,0));

	S_Collision lastCollisionY = getLastCollisionY();

	lastCollisionY.BlockType = CBlock::AIR;
	lastCollisionY.fBouncingFactorX = 0.0f;
	lastCollisionY.fBouncingFactorY = 0.0f;
	lastCollisionY.bIsCollision = false;

	setLastCollisionY(lastCollisionY);

	m_Alive = true;
	CLogfile::get()->fTextout("<br />New Worm. ID:%i",m_WormID);

	m_pSettings = &(g_pSettings->s.WormSet[m_WormID]);

}

void CWorm::reset() { //HINT: resettet nicht die Position
	setDir(CVec(0,0));
	m_Money = 0;
	m_Points = 0;
	m_Energy = MAXENERGY;
}

void CWorm::render() {
	CVec newWormPos = CVec(getRect());
	m_pWormSprite->SetPos( newWormPos);
	m_pWormSprite->Render(m_fAnimphase, m_bOrientation, m_TeamID);
}

void CWorm::ProcessMoving() {
	CVec newDir = getDir();
	if (g_pFramework->KeyDown(m_pSettings->KeyJump) && getCanJump() && !m_bJumpKeyLock) { //Jump!
		m_bJumpKeyLock = true;

		S_Collision lastCollisionY = getLastCollisionY();

		if (lastCollisionY.BlockType == CBlock::JUMPBOARD)
			newDir.y = 1.25f*WORMJUMPSPEED_Y;
		else
			newDir.y = WORMJUMPSPEED_Y;
	}

	if (!g_pFramework->KeyDown(m_pSettings->KeyJump))
		m_bJumpKeyLock = false;


	m_isWalking = false;
	//Left or Right!!
	if (g_pFramework->KeyDown(m_pSettings->KeyLeft) == true) {
		newDir.x += -WORMACCELERATION;
		m_isWalking = true;
		m_bOrientation = OLEFT;
		if (newDir.x < -WORMMAXSPEED_X) newDir.x = -WORMMAXSPEED_X;
	} else if (g_pFramework->KeyDown(m_pSettings->KeyRight) == true ) {
		newDir.x += +WORMACCELERATION;
		m_isWalking = true;
		m_bOrientation = ORIGHT;
		if (newDir.x > WORMMAXSPEED_X) newDir.x = WORMMAXSPEED_X;
	}

	setDir(newDir);
}

void CWorm::ProcessBuilding() {
	/*
	 * KeyDown:	MineBlock
	 * STRG:	BuildBlock
	 * Shift:	SelectBuildBlockType
	 */

	if (g_pFramework->KeyDown(m_pSettings->KeyMine)) {
		//get Block under Worm!
		CVec vec(getRect());
		vec.x += getRect().w/2;//Block UNDER worm
		CBlockKoord pos = vec.toBlockKoord();
		pos.y++;//Block UNDER worm
		CBlock* miningBlock = m_pGame->getBlock(pos);//returns NULL if for example out of Gameboard
		if (miningBlock != NULL) {
			int miningBlockTeamID = miningBlock->getTeamID();
			if ( ((miningBlockTeamID == NOBODY) || (miningBlockTeamID == m_TeamID))//you can't mine other teams blocks
					&& (miningBlock->getBlockType() != CBlock::AIR) ){
				int newMoney = m_Money + CBlock::BlockCosts[miningBlock->getBlockType()]; //da blocktype sich dann ja ändert... bei buildblock
				if (m_pGame->BuildBlock(pos, CBlock::AIR, m_WormID, m_TeamID)) { //block konnte gebaut werden!:
					m_Money = newMoney;
					m_Points++;
					g_pLogfile->fTextout(BLUE, false, "Mined Block");
				}
			}
		} else g_pLogfile->Textout("<br /> Couldn't mine Block because miningBlock == NULL");
	}//Keydown

	if ( (g_pFramework->KeyDown(m_pSettings->KeySelectBlockType)) && (m_bNextBTypeKeyLock == false) ) {
		m_selectedBType = CBlock::nextBlockType(m_selectedBType);
		m_bNextBTypeKeyLock = true;
	}
	if (!g_pFramework->KeyDown(m_pSettings->KeySelectBlockType))
		m_bNextBTypeKeyLock = false;

	if ( (g_pFramework->KeyDown(m_pSettings->KeyBuild))
			&& (m_bBuildKeyLock == false)
			&& (m_selectedBType != CBlock::AIR)//Build Air has no sense...
			&& (m_Money >= CBlock::BlockCosts[m_selectedBType]) //player has enough money
			) {
		m_bBuildKeyLock = true;
		//get field next to worm
		CVec vec(getRect());
		if (m_bOrientation == ORIGHT)
			vec.x += (getRect().w + BLOCKSIZE);//next block
		else
			vec.x -= BLOCKSIZE;

		CBlockKoord pos = vec.toBlockKoord();

		//is field free???
		CBlock* buildingBlock = m_pGame->getBlock(pos);
		if ( (buildingBlock != NULL)
				&& (buildingBlock->getBlockType() == CBlock::AIR)
				&& (g_pPhysics->isEmpty(pos)) ) {
			if (m_pGame->BuildBlock(pos, m_selectedBType, m_WormID, m_TeamID)) {
				g_pLogfile->fTextout("</br >Built BLock: "+CBlock::BlockTypeString(m_selectedBType)+" Costs:%i", CBlock::BlockCosts[m_selectedBType]);
				m_Money -= CBlock::BlockCosts[m_selectedBType];
				m_Points++;
			}

		}

	}
	if (!g_pFramework->KeyDown(m_pSettings->KeyBuild))
			m_bBuildKeyLock = false;


}

void CWorm::ProcessAnim() {
	if (m_isWalking)
		m_fAnimphase += 10.0f*g_pTimer->getElapsed();
	if (m_fAnimphase >= 3.0f)
		m_fAnimphase -= 3.0f;
}

void CWorm::ProcessView() {
	FloatRect WormRect = getRect();
	SDL_Rect ViewRect  = g_pFramework->ViewPorts.at(m_ViewPort).m_View;//MBE at nimmt performance raus!

	const int scrollForeward = static_cast<int>(ViewRect.w/2)+100;
	const int scrollBackward = static_cast<int>(ViewRect.w/2)-150;

	if (static_cast<int>(WormRect.x) > ViewRect.x+scrollForeward) {
		//move View rect that worm is at 300
		ViewRect.x = static_cast<int>(WormRect.x) - scrollForeward;
	}

	if (static_cast<int>(WormRect.x) < ViewRect.x + scrollBackward)
		ViewRect.x = static_cast<int>(WormRect.x) - scrollBackward;

	//handle right Border of World
	if (ViewRect.x + ViewRect.w > m_pGame->getWorldRect().w)
		ViewRect.x = m_pGame->getWorldRect().w - ViewRect.w;

	//handle left Border of World
	if (ViewRect.x < 0)
		ViewRect.x = 0;

	g_pFramework->ViewPorts[m_ViewPort].m_View = ViewRect;//MBE per at????

	char buffer[1024];
	sprintf(buffer, "Money: %iEur, Points: %i, Energy: %.0f/%.0f",m_Money, m_Points, m_Energy, MAXENERGY);
	string s = buffer;
	g_pFramework->TextOut(s, 0, 0, m_ViewPort);
	g_pFramework->TextOut(m_pSettings->name, 0, 70, m_ViewPort);

	s = "::"+CBlock::BlockTypeString(m_selectedBType)+"::";
	g_pFramework->TextOut(s, 0, 15, m_ViewPort);

	int BlockEnergyState = static_cast<int>(100*(g_pTimer->now()-m_fLastActionTime)/LOADINGTIME);
	if (BlockEnergyState > 100) BlockEnergyState = 100;
	sprintf(buffer, "BlockEnergy: %i %%", BlockEnergyState);
	s = buffer;
	g_pFramework->TextOut(s, 0, 30, m_ViewPort);
}

CWorm::~CWorm() {
	if (m_pWormSprite != NULL) {
		delete (m_pWormSprite);
	}
	m_pWormSprite = NULL;
	m_pGame = NULL;
	m_pSettings = NULL;


	CLogfile::get()->fTextout("<br />Deleted Worm; ID:%i",m_WormID);
}

string CWorm::getWormColorString() {
	string result;
	switch (m_Color) {
		case WC_RED:
			result = "red";
			break;
		case WC_BLUE:
			result = "blue";
			break;
		case WC_GREEN:
			result = "green";
			break;
		case WC_YELLOW:
			result = "yellow";
			break;
		default:
			result = "noColor!";
			CLogfile::get()->Textout("Couldn't get the Colorstring!<br />");			
			break;
	}
	return result;
}

void CWorm::update() {
	ProcessMoving();

	ProcessBuilding();

	ProcessBlockActions();

	ProcessAnim();

	//Physics happens in do physics!
}

bool CWorm::isAlive() {
	return m_Alive;
}

void CWorm::ProcessBlockActions() {
	bool canDoBlockAction = (g_pTimer->now()-m_fLastActionTime > LOADINGTIME);
	if (g_pFramework->KeyDown(m_pSettings->KeyBlockActions) && canDoBlockAction) {
		map<CBlockKoord, CBlock*>::iterator mIt;
		vector<CWorm*>::iterator wIt;
		CVec dist, block;

		m_fLastActionTime = g_pTimer->now();

		//FIXME: handle this with a special vector that contains Pointers to all blocks build by worm!!!
		//^^but that would take more RAM....
		for (mIt = m_pGame->m_Gameboard.begin(); mIt != m_pGame->m_Gameboard.end(); ++mIt) {
			if (mIt->second->getBuilderID() == m_WormID) {
				//Process actions for that block...
				if (mIt->second->getBlockType() == CBlock::SHOOTING) {
					for (wIt = m_pGame->m_vWorms.begin(); wIt != m_pGame->m_vWorms.end(); ++wIt) {
						//damage all near worms!!;
						if ((*wIt)->getTeamID() != m_TeamID) { //opponent!!!
							dist = (*wIt)->getRect();
							block = CVec(mIt->first);
							dist-= block;

							if (dist.quad_abs()<QUADSHOOTINGBLOCKRANGE) {
								float e;
								e = (*wIt)->getEnergy()-SHOOTINGBLOCKDAMAGE;
								(*wIt)->setEnergy(e);
								//DrawAttackAnimation
								//get midWorm, midBlock
								block.x += BLOCKSIZE/2;
								block.y += BLOCKSIZE/2;
								CAA_Laser * pLaser = new CAA_Laser();
								pLaser->init(1.5f, block, (*wIt), 255, 0, 0);
								m_pGame->m_AttackAnimations.push_back(pLaser);
								//earn points!
								changePointsBy(10);
								changeMoneyBy(5);
							}

						}
						//TODO: worms have to die if hp<0!

					}
				}//Shooting
				if (mIt->second->getBlockType() == CBlock::CANNON) {
					float minDist=0.01f;//MBE: hope thats beig enoough
					CWorm * pMinDistWorm = NULL;
					//findNearest opponent Worm:
					for (wIt = m_pGame->m_vWorms.begin(); wIt != m_pGame->m_vWorms.end(); ++wIt) {
						if ((*wIt)->getTeamID() != m_TeamID) { //opponent!!!
							dist  = (*wIt)->getRect();
							block = CVec(mIt->first);
							dist -= block;
							if (dist.quad_abs()<minDist || minDist <= 0.1f) {
								pMinDistWorm = (*wIt); //aktueller wurm am nächsten...
							}
						}
					}
					//set nearest worm as aim!!;
					CAA_CannonBall * pCannonBall = new CAA_CannonBall();
					pCannonBall->init(block, pMinDistWorm, this, m_TeamID,
							&(m_pGame->m_AttackAnimations));
					m_pGame->m_AttackAnimations.push_back(pCannonBall);

					pMinDistWorm = NULL;

				}//Cannon

			}//Worm built block


		}//for all Blocks

		//TODO: not here: Lightning!
		CAA_Cloud * pCloud = new CAA_Cloud();
		pCloud->init(CVec(0,0), this, this, 0);
		m_pGame->m_AttackAnimations.push_back(pCloud);
		pCloud = NULL;


	}

}
