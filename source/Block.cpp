#include "Block.hpp"


const int CBlock::BlockCosts[BLOCKAMOUNT]    	= BLOCKCOSTS;//TODO
const float CBlock::BlockBouncingX[BLOCKAMOUNT] = BLOCKBOUNCING_X;
const float CBlock::BlockBouncingY[BLOCKAMOUNT] = BLOCKBOUNCING_Y;
CSprite * CBlock::m_pBlockSprites[BLOCKAMOUNT];

//DECLARATION: static CSprite * m_pBlockSprites[5];
void CBlock::InitBlockSprites() {
	m_pBlockSprites[0] = new CSprite(_DIRDATA_+"/BlockAir.bmp");
	m_pBlockSprites[1] = new CSprite(_DIRDATA_+"/BlockNormal.bmp");
	m_pBlockSprites[2] = new CSprite(_DIRDATA_+"/BlockShooting.bmp");
	m_pBlockSprites[3] = new CSprite(_DIRDATA_+"/BlockJumpboard.bmp");
	m_pBlockSprites[4] = new CSprite(_DIRDATA_+"/BlockInvisible.bmp");
	m_pBlockSprites[5] = new CSprite(_DIRDATA_+"/BlockCannon.bmp");

}

void CBlock::FreeBlockSprites() {
	for (int i=0; i<5; i++) {
		if (m_pBlockSprites[i] != NULL) {
			delete (m_pBlockSprites[i]);
			m_pBlockSprites[i] = NULL;
		}
	}
}

CBlock::CBlock(BlockType BT) {
	m_BlockType = BT;
}

void CBlock::render(CBlockKoord &MyBlockPos) {
	CVec vec(MyBlockPos);
	if (this->m_BlockType != CBlock::AIR) {
		CBlock::m_pBlockSprites[this->m_BlockType]->SetPos(vec);
		CBlock::m_pBlockSprites[this->m_BlockType]->Render(this->m_TeamID+1);
	}
}

std::string CBlock::BlockTypeString(CBlock::BlockType bt) {
	std::string s;
	switch (bt) {
		case (CBlock::AIR): 		s="AIR"; 		break;
		case (CBlock::INVISIBLE): 	s="INVISIBLE"; 	break;
		case (CBlock::JUMPBOARD): 	s="JUMPBOARD"; 	break;
		case (CBlock::NORMAL): 		s="NORMAL"; 	break;
		case (CBlock::SHOOTING): 	s="SHOOTING";	break;
		case (CBlock::CANNON): 		s="CANNON";	break;
		default: 					s="None"; 		break;
	}
	return s;
}

CBlock::BlockType CBlock::nextBlockType(CBlock::BlockType now) {//you can't select air!!!
	CBlock::BlockType s;
	switch (now) {
		case (CBlock::JUMPBOARD): 	s=CBlock::NORMAL; 		break;
		case (CBlock::NORMAL): 		s=CBlock::SHOOTING; 	break;
		case (CBlock::SHOOTING): 	s=CBlock::INVISIBLE;	break;
		case (CBlock::INVISIBLE): 	s=CBlock::CANNON;	 	break;
		case (CBlock::CANNON):	 	s=CBlock::JUMPBOARD; 	break;
		default: 					s=CBlock::NORMAL;		break;
	}
	return s;
}
