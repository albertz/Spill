#include "Game.hpp"

using namespace std;

CGame::CGame(int AmountOfPlayers, int GameBoardWidth, int GameBoardHeight) {
	if (AmountOfPlayers > 4) {
		AmountOfPlayers = 4;
		CLogfile::get()->Textout("Can't Create so many Players, will create 4<br />");
	}

	for (int i=1; i<=AmountOfPlayers; i++) {//w�rmer auff�llen
		CWorm* pWorm = new CWorm();
		pWorm->init(i, 40.0f, 40.0f, CWorm::WC_RED); //wir machen alle Würmer rot....
		m_vWorms.push_back(pWorm); //MBE: evtl Teams
	}
	m_WormAmount = AmountOfPlayers;

	if (GameBoardWidth > MAXGAMEBOARDWIDTH) {
		m_GBWidth = MAXGAMEBOARDWIDTH;
		CLogfile::get()->Textout("Can't Create Gameboard with this WIDTH, will create Gameboard with width MAXGAMEBOARDWIDTH<br />");
	} else
		m_GBWidth = GameBoardWidth;

	if (GameBoardHeight > MAXGAMEBOARDHEIGHT) {
		m_GBHeight = MAXGAMEBOARDHEIGHT;
		CLogfile::get()->Textout("Can't Create Gameboard with this HEIGHT, will create Gameboard with HEIGHT MAXGAMEBOARDHEIGHT<br />");
	} else
		m_GBHeight = GameBoardHeight;

	g_pLogfile->Textout("Created Gameboard <br />");

	
	//map<CKoord, CBlock*>::iterator it; den brauch ma hier ncih
	pair<CBlockKoord, CBlock*> Gamefield;
	for (int x=0; x<m_GBWidth; x++) {	//Das gesamte Gameboard zu Luft machen
		for (int y=0; y<m_GBHeight; y++) {	
			CBlockKoord pos(x,y);						
			Gamefield = make_pair( pos, new CBlock(CBlock::AIR));
			m_Gameboard.insert(Gamefield);
		}
	}
	g_pLogfile->Textout("Gameboard filled with NULL... <br />");
	//TODO: load World...
	//ok infact might be good to generate an sensfull empty World at first:
	//with a GROUND!
	map<CBlockKoord, CBlock*>::iterator it;
	for (int x = 0; x < m_GBWidth; x++) {
		CBlockKoord pos(x,18);
		it = m_Gameboard.find(pos);
		if (it != m_Gameboard.end()) {
			it->second->setBlockType(CBlock::NORMAL);
			it->second->setBuilderID(-1); //BuilderID -1 means its from levelbuilder!
			it->second->setTeamID(-1);
		}
	}

	CBlock::InitBlockSprites();

	m_bIsRunning = true;

}

///////////////////////////////////////////////////////////////////////////////////
void CGame::run() {
	while (m_bIsRunning) {
		//Spielen!

		//tasten holen, screen flippen, Zeit holen:
		g_pFramework->Clear();
		g_pFramework->Update();
		g_pTimer->Update();
		ProcessEvents();//reagiert auf excape usw zum beenden!
		//W�rmer verschieben, abbremsen usw.
		CPhysics::doPhysics(this);

		//render Gameboard before you render worms xD
		renderGameboard();

		//w�rmer zeichnen!
		vector<CWorm*>::iterator i;
		for (i = m_vWorms.begin(); i<m_vWorms.end(); i++) {
			(*i)->update();
			(*i)->render();
		}

		g_pFramework->RenderDebugText();
		g_pFramework->Flip();
	}
}
////////////////////////////////////////////////////////////////////////////////////

void CGame::renderGameboard() {
	map<CBlockKoord, CBlock*>::iterator it;
	for (it=m_Gameboard.begin() ; it!=m_Gameboard.end(); ++it) {//alle Bl�cke rendern!
		CBlockKoord pos = it->first;
		it->second->render(pos);
	}
}

void CGame::ProcessEvents() {
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case (SDL_QUIT): {
				m_bIsRunning = false;
			} break;
			case (SDL_KEYDOWN): {
				switch (event.key.keysym.sym) {
					case (SDLK_ESCAPE): {
						m_bIsRunning = false;
					} break;
					default: //nothing
						break;
				} //switch event.key.keysym.sym
			} break;
		} //Switch event.type
	} //if
}

void CGame::quit() {
	//DeleteWorms
	//free Gameboard
	//free Blockimages!

	vector<CWorm*>::iterator wit;
	for (wit = m_vWorms.begin(); wit!=m_vWorms.end(); wit++) {
		if ((*wit)!=NULL) {
			delete ((*wit));
			(*wit) = NULL;
		}
	}
	m_vWorms.clear();

	map<CBlockKoord, CBlock*>::iterator mit;
	for (mit = m_Gameboard.begin(); mit != m_Gameboard.end(); ++mit) {
		if (mit->second != NULL) {
			delete (mit->second);
			mit->second = NULL;
		}
	}

	CBlock::FreeBlockSprites();
}

CGame::~CGame() {
	//nichts :P
	CLogfile::get()->Textout("Destroyed CGame </ br>");
}

int CGame::getGBHeight() const
{
    return m_GBHeight;
}

int CGame::getGBWidth() const
{
    return m_GBWidth;
}

void CGame::setGBHeight(int m_GBHeight)
{
    this->m_GBHeight = m_GBHeight;
}

void CGame::setGBWidth(int m_GBWidth)
{
    this->m_GBWidth = m_GBWidth;
}

