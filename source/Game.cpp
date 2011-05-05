#include "Game.hpp"
#include "Settings.hpp"

#include "Performancecheck.hpp"

using namespace std;

CGame::CGame(int AmountOfPlayers, int GameBoardWidth, int GameBoardHeight) {//TODO: cut in more functions!

	if (AmountOfPlayers > 4) {
		AmountOfPlayers = 4;
		CLogfile::get()->Textout("Can't Create so many Players, will create 4<br />");
	}

	g_pFramework->InitViewPorts(AmountOfPlayers);

	for (int i=0; i<AmountOfPlayers; i++) {//w�rmer auff�llen
		CWorm* pWorm = new CWorm(this);
		pWorm->init(i, i, 40.0f, 40.0f, CWorm::WC_RED); //wir machen alle Würmer rot....
		//MBE WormID=TeamID
		pWorm->setName(g_pSettings->WormSet[i].name);
		pWorm->setViewPort(i);
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

	m_WorldRect.x = 0;
	m_WorldRect.y = 0;
	m_WorldRect.w = m_GBWidth  * BLOCKSIZE;
	m_WorldRect.h = m_GBHeight * BLOCKSIZE;

	g_pLogfile->Textout("Created Gameboard <br />");

	
	pair<CBlockKoord, CBlock*> Gamefield;
	for (int x=0; x<m_GBWidth; x++) {	//fill the whole Gameboard with air
		for (int y=0; y<m_GBHeight; y++) {	
			CBlockKoord pos(x,y);						
			Gamefield = make_pair( pos, new CBlock(CBlock::AIR));
			Gamefield.second->setBuilderID(NOBODY);
			Gamefield.second->setTeamID(NOBODY);
			m_Gameboard.insert(Gamefield);
		}
	}
	g_pLogfile->Textout("Gameboard filled with NULL... <br />");
	//TODO: load World...
	//or Take DebugWorld:
	creatDebugGameBoard();


	CBlock::InitBlockSprites();

	m_pBackGround = new CBackGround();
	m_pBackGround->init(GameBoardWidth * BLOCKSIZE);

	m_bIsRunning = true;
	g_pLogfile->Textout(RED, true, "Constructed CGame");
}

void CGame::creatDebugGameBoard() {//creates World for debugging
	//with a GROUND!
	for (int x = 0; x < m_GBWidth; x++) {
		CBlockKoord pos(x,18);
		BuildBlock(pos, CBlock::NORMAL, NOBODY, NOBODY);
	}

	//build a wall
	for (int y = 2; y < m_GBHeight-10; y++) {
		CBlockKoord pos(22,y);
		BuildBlock(pos, CBlock::SHOOTING, NOBODY, NOBODY);
	}

	//build a single block
	CBlockKoord pos(14, 14);
	BuildBlock(pos, CBlock::JUMPBOARD, NOBODY, NOBODY);
	pos.x = 3;
	BuildBlock(pos, CBlock::JUMPBOARD, NOBODY, NOBODY);

	pos.x = 4;
	pos.y = 17;
	BuildBlock(pos, CBlock::INVISIBLE, NOBODY, NOBODY);

	//"Stairs"
	pos.x = 19;
	pos.y = 14;
	BuildBlock(pos, CBlock::JUMPBOARD, NOBODY, NOBODY);
	pos.x = 20;
	pos.y = 10;
	BuildBlock(pos, CBlock::JUMPBOARD, NOBODY, NOBODY);

}



///////////////////////////////////////////////////////////////////////////////////
void CGame::run() {
	while (m_bIsRunning) {
		SDL_Delay(20);

		//Play!
		//g_pFramework->showDebugValue("Fps: %.1f", 1/(g_pTimer->getElapsed()) );

		//nimmt unwesentliche Zeit von 1ms:

		startWatch();

		//m_pBackGround->render();FIXME uncomment when rendering works!
		stopWatch("rendered back");
		g_pFramework->Update();//Update Timer and Framework!
		ProcessEvents();//react on escape for close...
/*

		startWatch();
		//Move Worms, slow them down and so on...
		g_pPhysics->doPhysics();//schnell!!
		stopWatch("doPhysics");

		startWatch();
		//render gameboard before you render worms xD
		renderGameboard();
		stopWatch("renderGameBoard");



		startWatch();
		//Draw Worms and react on keys...
		vector<CWorm*>::iterator i;
		for (i = m_vWorms.begin(); i!=m_vWorms.end(); i++) {
			(*i)->update();
			(*i)->ProcessView();
			(*i)->render();
		}
		stopWatch("draw worms");

		g_pFramework->renderViewPortFrames();

		g_pFramework->RenderDebugText();
*/
		g_pFramework->Clear(); //Clear current surface
		//Lösch mich:
		//ok lass uns mal die ogl testen...
		    glLoadIdentity();

		    glBegin(GL_QUADS);
		        glColor3f(1, 0, 0); glVertex3f(0, 0, 0);
		        glColor3f(1, 1, 0); glVertex3f(100, 0, 0);
		        glColor3f(1, 0, 1); glVertex3f(100, 100, 0);
		        glColor3f(1, 1, 1); glVertex3f(0, 100, 0);
		    glEnd();

		    SDL_GL_SwapBuffers();
	}
}
////////////////////////////////////////////////////////////////////////////////////

void CGame::renderGameboard() {
	map<CBlockKoord, CBlock*>::iterator it, s, e;
	for (it=m_Gameboard.begin(); it!=m_Gameboard.end(); ++it) {//alle Bl�cke rendern!
		CBlockKoord pos = it->first;
		//startWatch();
		try {
			it->second->render(pos);
		} catch (exception &e) {
			g_pLogfile->fTextout(RED,"Error on creating gameboard: %s", e.what());
		}
		//stopWatch("Watch");
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
	m_Gameboard.clear();

	CBlock::FreeBlockSprites();
	m_pBackGround->quit();
	CLogfile::get()->Textout("</ br>quitted CGame");


}

CBlock* CGame::getBlock(CBlockKoord Where) {
	map<CBlockKoord, CBlock*>::iterator it;
	it = m_Gameboard.find(Where);
	if (it != m_Gameboard.end()) {
		return (it->second);
	} else return NULL;
}

bool CGame::BuildBlock(CBlockKoord Where, CBlock::BlockType Type, int BuilderID, int TeamID) {
	CBlock *pBlock = getBlock(Where);
	if (pBlock != NULL) {
		pBlock->setBlockType(Type);
		pBlock->setBuilderID(BuilderID);
		pBlock->setTeamID(TeamID);
		return true;
	} else return false;
}

CGame::~CGame() {}

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
