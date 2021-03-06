#include "Physics.hpp"
#include <list>
#include "AttackAnimations/AttackAnimation.hpp"

using namespace std;

void CPhysics::doPhysicsFor(CPhysicalObject * it) {

	if (! it->getCanMove())
		return;

	S_Collision YCollision;
	S_Collision XCollision;

	float timeElapsed = g_pTimer->getElapsed();
	if (timeElapsed == 0.0f)
		return; //macht keinen Sinn....
	FloatRect FR = it->getRect(); //rect of PO..PhysicalObject
	CVec dir = it->getDir(); //dir of PO
	XCollision.bIsCollision = false;
	YCollision.bIsCollision = false;

	float time = timeElapsed;
	int i;//Anzahl der Kleinschritte herrausbekommen:
	for (i = 1; dir.quad_abs()*time > BLOCKSIZE*BLOCKSIZE; i++) {
		time = timeElapsed/i;
	}

	////////////////////
	//X/Y-Collisions:
	for (i = 1; (i*time <= timeElapsed); i++) {
		if (!XCollision.bIsCollision) {
			FR.x += dir.x*time;
			XCollision = getCollision(FR);
			if (XCollision.bIsCollision) { //kollission durch x-Rutschen?
				FR.x -= dir.x*time; //dann x-Rutschen wieder r�ckg�ngig machen
				dir.x *= (-1 * XCollision.fBouncingFactorX);
			}
		}

		if (!YCollision.bIsCollision) {
			//yes all physicall objects fall!
			//Fallbeschleunigung dazu!
			//HINT: Fallkurve hängt von getTimeelapsed ab!! evtl mit s=g/2t² arbeieten
			dir.y += Gravity*time; //graviy muss nach unten zeigen...


			it->setCanJump(false); //kann auf jeden erstmal nicht springen
			//Kollission durch y-Verschiebung??
			FR.y += dir.y*time;
			YCollision = getCollision(FR);
			if (YCollision.bIsCollision) { //kollission durch y-Rutschen?
				//JumpingBoard...
				//getBouncingfactor from Blocktype
				FR.y -= dir.y*time; //dann y-Rutschen wieder r�ckg�ngig machen
				dir.y *= (-1 * YCollision.fBouncingFactorY);//neues Y
				if ((Abs(dir.y) < 10.0f) || (YCollision.BlockType == CBlock::JUMPBOARD)) {
					it->setCanJump(true);
				}
			}

		}

		if (YCollision.bIsCollision && XCollision.bIsCollision)
			break;

	}//für jeden SChritt...
	//wenn keine x-Kollission:
	//HINT:Reibung://Flugreibung ist Sinnlos!
	if (!XCollision.bIsCollision)
		dir.x *= (Friction ) ; //TODO TimeElapsed einrechnen!

	if (FR.x < 0.0f) FR.x = 0.0f; //man kann nicht aus dem linken bildschirm fallen!!
	if (FR.y < 0.0f) {//opben geht das ganze natürlich auch nicht
		FR.y = 0.0f;
		dir.y = 0.0f;
	}//MBE Performance: allways 0.0f if its float! with .0f!!!



	//Wenn keine kollission dann Verschieben !...^^
	it->setDir(dir);
	it->setRect(FR);

	//gegebenenfalls neue collision setzen
	if (YCollision.BlockType != CBlock::AIR)
		it->setLastCollisionY(YCollision);
}





bool CPhysics::doPhysics() {
	//TODO: falls es noch andere solid-physicalobjets gibt, diese berücksichtigen!!!

	vector<CWorm*>::iterator it;
	for (it = m_pGame->m_vWorms.begin(); it!=m_pGame->m_vWorms.end(); ++it) {
		if ( !((*it)->getCanMove() && (*it)->isAlive()) )
			break; //auf zum n�chsten Wurm!
		doPhysicsFor((*it));


	}//FÜPR JEDEN WURM

	//für jede attack:

	list<CAttackAnimation*>::iterator Ait;
	for (Ait = m_pGame->m_AttackAnimations.begin(); Ait != m_pGame->m_AttackAnimations.end(); ++Ait) {
		if ( ((*Ait)->getCanMove())) {
			doPhysicsFor((*Ait));
		}
	}

	//do physics for every item!
	list<CItem*>::iterator Iit;
	for (Iit = m_pGame->m_pItems.begin(); Iit != m_pGame->m_pItems.end(); ++Iit) {
		if ((*Iit)->getCanMove())
			doPhysicsFor(*Iit);
	}

	return true;
}

CBlock::BlockType CPhysics::getBlockType(CVec &vec) {
	CBlock* b;
	b= m_pGame->getBlock(vec.toBlockKoord());
	if (b != NULL) //vec existiert tats�chlich!
		return b->getBlockType();
	else
		return CBlock::NORMAL;
}

bool CPhysics::rectCollision(const FloatRect &FR1, const FloatRect &FR2) { //überprüft ob sich zwei rects schneiden!
	return ( (FR1.y < FR2.y+FR2.h) && (FR1.y+FR1.h > FR2.y)
			&& (FR1.x < FR2.x+FR2.w) && (FR1.x+FR1.w > FR2.x) );
}

S_Collision CPhysics::getCollision(const FloatRect &FR) {
	#define CHECKPOINTS 4//jez reichen 4 Punke, da Worm kleiner Block!!

	CVec vecs[CHECKPOINTS];

	vecs[0] = CVec (FR);
	vecs[1] = CVec (FR.x+FR.w, FR.y);
	vecs[2] = CVec (FR.x, FR.y+FR.h);
	vecs[3] = CVec (FR.x+FR.w, FR.y+FR.h);
	/*vecs[4] = CVec (FR.x+FR.w/2, FR.y);
	vecs[5] = CVec (FR.x+FR.w/2, FR.y+FR.h);*/


	//TODO USE RECTCOLLISION!!!
	//HINT: reicht z.z.T wenn wir die Ecken + 2 Mitten überprüfen, da unser Worm maximal auf vier verschiedenen Feldern Sein kann!!
	S_Collision result;//init Result:
	result.fBouncingFactorX = 0.0f;
	result.fBouncingFactorY = 0.0f;
	result.bIsCollision = false;
	result.BlockType = CBlock::AIR;



	for (int i=0; i<CHECKPOINTS; i++) {
		CBlock::BlockType curType = CPhysics::getBlockType(vecs[i]);
		if (curType != CBlock::AIR)
			result.bIsCollision = true;
		if (CBlock::BlockBouncingX[curType] > result.fBouncingFactorX)
			result.fBouncingFactorX = CBlock::BlockBouncingX[curType];
		if (CBlock::BlockBouncingY[curType] > result.fBouncingFactorY)
			result.fBouncingFactorY = CBlock::BlockBouncingY[curType];
		if (curType > result.BlockType) {
			result.BlockType = curType;
		}
	}


	return result;
}

void CPhysics::init(CGame * game) {
	m_pGame = game;
	g_pLogfile->Textout("</br>inited Physics");
}

bool CPhysics::isEmpty(CBlockKoord &bc) {
	//tests whether no other Worm is on the Field!
	vector<CWorm*>::iterator it;
	for (it = m_pGame->m_vWorms.begin(); it != m_pGame->m_vWorms.end(); ++it) {
		CVec vec(bc);
		FloatRect blockRect = vec.toBlockFloatRect();

		FloatRect wormRect = (*it)->getRect();
		if (rectCollision(blockRect, wormRect)==true)//verdaaaaammmmmtes ; war hier!!!
			return false;
	}
	return true;
}


