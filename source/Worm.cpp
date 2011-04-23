#include "Worm.hpp"
#include <map>

using namespace std;

CWorm::CWorm(CGame *pGame) {
	m_pWormSprite = NULL;
	m_pGame = pGame;
}

void CWorm::init(int WormID) {
	init(WormID, 0, 0, WC_RED);
}

void CWorm::init(int WormID, float X, float Y) {
	init(WormID, X, Y, WC_RED);
}

void CWorm::init(int WormID, WORMCOLORS WC){
	init(WormID, 0, 0, WC);
}


void CWorm::init(int WormID, float X, float Y, WORMCOLORS WC) {
	m_WormID = WormID;
	m_TeamID = 0; //MBE
	setCanJump(false);
	m_Color = WC;
	m_Money = 0;
	m_Points = 0;
	m_Energy = MAXENERGY;
	setCanMove(true);
	
	m_bNextBTypeKeyLock = false;
	m_selectedBType = CBlock::AIR;

	m_bOrientation = ORIGHT;

	m_Name = ""; //getName(); --> TODO!!! Abfrage per Eingabe vom User!!!!
	
	m_pWormSprite = new CSprite();
	//m_pWormSprite->Load(_DIRDATA_+"/christmaswormjustwalk.bmp", 21, 32, 17);
	//New animation: //braucht auch anpassung in Animate!
	m_pWormSprite->Load(_DIRDATA_+"/christmaswormjustwalk.bmp", 3 , 25, 18);
	m_pWormSprite->SetColorKey(255, 0, 255);
	m_fAnimphase = 0.0f;
	m_isWalking = false;

	FloatRect FR;
	FR = m_pWormSprite->GetRect();
	setRect(FR);
	setDir(CVec(0,0));

	m_lastCollisionY.BlockType = CBlock::AIR;
	m_lastCollisionY.fBouncingFactorX = 0.0f;
	m_lastCollisionY.fBouncingFactorY = 0.0f;
	m_lastCollisionY.bIsCollision = false;

	m_Alive = true;
	CLogfile::get()->fTextout("New Worm; ID:%i<br />",m_WormID);
}

S_Collision CWorm::getLastCollisionY() const
{
    return m_lastCollisionY;
}

void CWorm::setLastCollisionY(S_Collision &m_lastCollisionY)
{
    this->m_lastCollisionY = m_lastCollisionY;
}

void CWorm::reset() { //HINT: resettet nicht die Position
	setDir(CVec(0,0));
	m_Money = 0;
	m_Points = 0;
	m_Energy = MAXENERGY;
}

void CWorm::render() {
	CVec newWormPos = CVec(getRect());
	m_pWormSprite->SetPos( newWormPos );
	m_pWormSprite->Render(m_fAnimphase);
}

void CWorm::ProcessMoving() {//FIXME nicht alle W�rmer d�rfen die selben Tasten nutzen!!!
	CVec newDir = getDir();
	if (g_pFramework->KeyDown(SDLK_UP) && getCanJump() && !m_bJumpKeyLock) { //Jump!
		//setCanJump(false); //TODO überflüssig da in physic eh immer erstmal false
		m_bJumpKeyLock = true;

		if (m_lastCollisionY.BlockType == CBlock::JUMPBOARD)
			newDir.y = 1.25f*WORMJUMPSPEED_Y;
		else
			newDir.y = WORMJUMPSPEED_Y;
	}

	if (!g_pFramework->KeyDown(SDLK_UP))
		m_bJumpKeyLock = false;


	m_isWalking = false;
	//Left or Right!!
	if (g_pFramework->KeyDown(SDLK_LEFT) == true) {
		newDir.x += -WORMACCELERATION;
		m_isWalking = true;
		m_bOrientation = OLEFT;
		if (newDir.x < -WORMMAXSPEED_X) newDir.x = -WORMMAXSPEED_X;
	} else if (g_pFramework->KeyDown(SDLK_RIGHT) == true ) {
		newDir.x += +WORMACCELERATION;
		m_isWalking = true;
		m_bOrientation = ORIGHT;
		if (newDir.x > WORMMAXSPEED_X) newDir.x = WORMMAXSPEED_X;
	}

	setDir(newDir);
}

void CWorm::ProcessBuilding() {
	//TODO!!+Processweapons!!! for attacks of othe blocks!
	/*
	 * KeyDown:	MineBlock
	 * STRG:	BuildBlock
	 * Shift:	SelectBuildBlockType
	 */

	if (g_pFramework->KeyDown(SDLK_DOWN)) {
		//get Block under Worm!
		CBlockKoord pos = CVec(getRect()).toBlockKoord();
		pos.y++; //Block UNDER worm
		pos.x += (m_bOrientation==ORIGHT ? 1 : 0); //Orientation
		CBlock* miningBlock = m_pGame->getBlock(pos);
		if (miningBlock->getBlockType() != CBlock::AIR) {
			int newMoney = m_Money + CBlock::BlockCosts[miningBlock->getBlockType()]; //da blocktype sich dann ja ändert... bei buildblock
			if (m_pGame->BuildBlock(pos, CBlock::AIR, m_WormID, m_TeamID)) { //block konnte gebaut werden!:
				m_Money = newMoney;
				m_Points++;
				g_pLogfile->fTextout(BLUE, false, "Mined Block for %iEur", CBlock::BlockCosts[miningBlock->getBlockType()]);
			}
		}
	}//Keydown

	if ( (g_pFramework->KeyDown(SDLK_LSHIFT)) && (m_bNextBTypeKeyLock == false) ) {//FIXME right key???
		m_selectedBType = CBlock::nextBlockType(m_selectedBType);//MBE überschlag
		m_bNextBTypeKeyLock = true;
	}
	if (!g_pFramework->KeyDown(SDLK_LSHIFT))
		m_bNextBTypeKeyLock = false;


}

void CWorm::ProcessAnim() {
	//TODO! Nicht immer nur die selbe animation
	//FIXME think of Orientation
	if (m_isWalking)
		m_fAnimphase += 10.0f*g_pTimer->getElapsed();
	if (m_fAnimphase >= 3.0f)
		m_fAnimphase -= 3.0f;
}

void CWorm::ProcessView() {
	FloatRect WormRect = getRect();
	SDL_Rect ViewRect  = g_pFramework->getViewRect();


	if (static_cast<int>(WormRect.x) > ViewRect.x+400) {
		//move View rect that worm is at 300
		ViewRect.x = static_cast<int>(WormRect.x) - 400;
	}

	if (static_cast<int>(WormRect.x) < ViewRect.x + 250)
		ViewRect.x = static_cast<int>(WormRect.x) - 250;

	if (ViewRect.x + ViewRect.w > g_pFramework->getWorldRect().w)
		ViewRect.x = g_pFramework->getWorldRect().w - ViewRect.w;

	if (ViewRect.x < 0)
		ViewRect.x = 0;

	g_pFramework->setViewRect(ViewRect);

	char buffer[1024];
	sprintf(buffer, "Money: %iEur,   Points: %i,   Energy: %i/%i",m_Money, m_Points, m_Energy, MAXENERGY);

	string s = "::"+CBlock::BlockTypeString(m_selectedBType)+"::"+buffer;

	g_pFramework->TextOut(s, 0, 0);
}

CWorm::~CWorm() {
	if (m_pWormSprite != NULL) {
		delete (m_pWormSprite);
	}
	m_pWormSprite = NULL;
	CLogfile::get()->fTextout("Deleted Worm; ID:%i<br />",m_WormID);
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

	ProcessAnim();

	//Physics happens in do physics!
}

bool CWorm::isAlive() {
	return m_Alive;
}
