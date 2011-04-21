#include "Worm.hpp"
#include "Block.hpp"

using namespace std;

CWorm::CWorm() {
	m_pWormSprite = NULL;
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
	setCanJump(false);
	m_Color = WC;
	m_Money = 0;
	m_Points = 0;
	m_Energy = MAXENERGY;
	setCanMove(true);
	
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
	m_lastCollisionY.BouncingFactorX = 0.0f;
	m_lastCollisionY.BouncingFactorY = 0.0f;
	m_lastCollisionY.bIsCollision = false;

	m_Alive = true;
	CLogfile::get()->fTextout("New Worm; ID:%i<br />",m_WormID);
}

S_Collision CWorm::getLastCollisionY() const
{
    return m_lastCollisionY;
}

void CWorm::setLastCollisionY(S_Collision m_lastCollisionY)
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
	char buffer[1024];
	sprintf(buffer, "Money: %iEur,   Points: %i,   Energy: %i/%i", m_Money, m_Points, m_Energy, MAXENERGY);
	g_pFramework->TextOut(string(buffer), 0, 0);
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
		if (newDir.x < -WORMMAXSPEED_X) newDir.x = -WORMMAXSPEED_X;
	} else if (g_pFramework->KeyDown(SDLK_RIGHT) == true ) {
		newDir.x += +WORMACCELERATION;
		m_isWalking = true;
		if (newDir.x > WORMMAXSPEED_X) newDir.x = WORMMAXSPEED_X;
	}
	setDir(newDir);
}

void CWorm::ProcessBuilding() {
	//TODO!!+Processweapons!!! for attacks of othe blocks!

}

void CWorm::ProcessAnim() {
	//TODO! Nicht immer nur die selbe animation
	if (m_isWalking)
		m_fAnimphase += 10.0f*g_pTimer->getElapsed();
	if (m_fAnimphase >= 3.0f)
		m_fAnimphase -= 3.0f;
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
