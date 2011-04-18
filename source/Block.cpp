#include "Block.hpp"


const int CBlock::BlockCosts[5] = {0, 5, 10, 10, 10}; //TODO!!!
void CBlock::InitBlockSprites() {
	//CBlock::m_pBlockSprites[5] = { //darf nciht const sein da ja ->render und so davon aufgerufen wird!
	CBlock::m_pBlockSprites[0] = new CSprite("BlockAir.bmp");
	CBlock::m_pBlockSprites[1] = new CSprite("BlockNormal.bmp");
	CBlock::m_pBlockSprites[2] = new CSprite("BlockShooting.bmp");
	CBlock::m_pBlockSprites[3] = new CSprite("BlockJumpboard.bmp");
	CBlock::m_pBlockSprites[4] = new CSprite("BlockInvisible.bmp");
}

void CBlock::FreeBlockSprites() {
	for (int i=0; i<5; i++) {
		if (CBlock::m_pBlockSprites[i] != NULL) {
			delete (CBlock::m_pBlockSprites[i]);
			CBlock::m_pBlockSprites[i] = NULL;
		}
	}
}

CBlock::CBlock(BlockType BT) {
	m_BlockType = BT;
}

void CBlock::render(CBlockKoord &BlockPos) {
	CVec vec(BlockPos);
	CBlock::m_pBlockSprites[this->m_BlockType]->SetPos(vec);
	CBlock::m_pBlockSprites[this->m_BlockType]->Render();
}
