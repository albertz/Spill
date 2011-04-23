#include "Framework.hpp"

using namespace std;

bool CFramework::Init(int ScreenWidth, int ScreenHeight, int ColorDepth, bool bFullscreen) {
	//alle Systeme die wir brauchen der SDL initialisieren
	if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1 ) {
		string describtion ("SDL konnte nicht initialisiert werden");
		describtion = describtion + SDL_GetError();

		g_pLogfile->FunctionResult("CFramework::Init", L_FAIL, describtion  );
		
		Quit();

		return (false);
	}

	//Fullscreen/Windowmode???

	if (bFullscreen) {
		m_pView = SDL_SetVideoMode (ScreenWidth, ScreenHeight, ColorDepth,
										SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
	} else {
		m_pView = SDL_SetVideoMode (ScreenWidth, ScreenHeight, ColorDepth,
										SDL_HWSURFACE | SDL_DOUBLEBUF);
	}

	if (m_pView == NULL) {
		string describtion ("Videomodus konnte nicht gesetzt werden");
		describtion = describtion + SDL_GetError();

		g_pLogfile->FunctionResult("CFramework::Init", L_FAIL, describtion  );
		
		Quit();

		return (false);
	}

	m_ScreenRect.x = 0;
	m_ScreenRect.y = 0;
	m_ScreenRect.w = ScreenWidth;
	m_ScreenRect.h = ScreenHeight;

	m_ViewRect.w = ScreenWidth;
	m_ViewRect.h = ScreenHeight;


	m_WorldRect.x = 0;
	m_WorldRect.y = 0;
	m_WorldRect.w = 0;//Are set in CGame again
	m_WorldRect.h = 0;

	//Tastenstatusarray ZEIGER belegen

	m_pKeystate = SDL_GetKeyState(NULL);
	
	//Init SFont
	g_pLogfile->Textout("did sth.");
	string s = _DIRDATA_; s+="/24P_Copperplate_Blue.bmp";
	g_pLogfile->Textout("<br />"+s);
	SDL_Surface * pFontImage = SDL_LoadBMP((_DIRDATA_+"/24P_Copperplate_Blue.bmp").c_str());
	SDL_SetColorKey(pFontImage, SDL_SRCCOLORKEY, SDL_MapRGB(pFontImage->format, 0, 0, 0)); //COLORKEY Black!
	pGameFont = SFont_InitFont(pFontImage);

	//Init Debugvalue
	m_DebugValue = "";

	return (true);
}

void CFramework::Quit() {
	SFont_FreeFont(pGameFont);
//MBE SDL_FreeSurface
	SDL_FreeSurface(m_pView);
	SDL_Quit();
}

void CFramework::Update() {
	//Timer...
	g_pTimer->Update();

	//Tastaturstatus ermittlen
	SDL_PumpEvents();
}

void CFramework::RenderDebugText() {
	//DebugText:
	if (m_DebugValue != "") {
		TextOut(m_DebugValue, 0,50);
	}
}

bool CFramework::KeyDown(int Key_ID) {
	return (m_pKeystate[Key_ID] ? true : false); //na das is ja mal sinnlos
}


void CFramework::Clear() {//Augabe: Buffer l�schen
	SDL_FillRect(m_pView, NULL, SDL_MapRGB(m_pView->format, 0,255,255));
	//fill Black
}

void CFramework::Flip() {//surface umschalten, flippen
	//SDL_
	SDL_Flip(m_pView);
}

void CFramework::showDebugValue(const string Text, ...) {
	char buffer[MAX_BUFFER];
	va_list pArgList;	//hierrein hauts jetzt die �bergebenen parameter!

	if (Text.length()+1>MAX_BUFFER) {
		g_pLogfile->FunctionResult("showDebugValue", L_FAIL, "*Text > MAX_BUFFER!");
		return;
	}

	va_start (pArgList, Text); //std::string aus Argumenten erstellen!
	vsprintf(buffer, Text.c_str(), pArgList);
	va_end (pArgList);


	//erzeugten std::string schreiben:
	m_DebugValue = buffer;
}

void CFramework::TextOut(std::string &text, int x, int y) {
	SFont_Write(m_pView, pGameFont, x, y, text.c_str());
}
void CFramework::TextOut(std::string &text, CVec &where) {
	int xx = static_cast<int>(where.x);
	int yy = static_cast<int>(where.y);

	SFont_Write(m_pView, pGameFont, xx, yy, text.c_str());
}

bool CFramework::RectInView(SDL_Rect rect) {
	//überprüft ob sich zwei rects schneiden!
	return ( (rect.y < m_ViewRect.y+m_ViewRect.h) && (rect.y+rect.h > m_ViewRect.y)
			&& (rect.x < m_ViewRect.x+m_ViewRect.w) && (rect.x+rect.w > m_ViewRect.x) );

}


