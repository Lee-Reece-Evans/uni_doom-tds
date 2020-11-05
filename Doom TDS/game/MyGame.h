#pragma once


#include "Game.h"
#include "Door.h"


class CMyGame : public CGame
{
public:
	CMyGame();
	~CMyGame();

    // ----  Declare your game variables and objects here -------------

	// Variables
	int score;
	int level;
	int shotgunammo;
	int pistolammo;

	double difficulty;

	// boolean
	bool hasbluekey;
	bool hasredkey;
	bool hasyellowkey;
	bool usingpistol;
	bool usingshotgun;
	bool collectedshotgun;
	bool levelcompleted; // completed current level - if true carrys over previous level ammo and health to next level
	bool puzzleactive; // for door on level 3
	bool helpscreenactive; // for displaying menu helpscreen

    // Models and Model Lists
    CModelMd2 player;   // animated md2 model
	CModelMd2 gun;
	CModelMd2 Imp;		// animated Imp md2 model 
	CModelMd2 impball;
	CModelMd2 twitcher; // animated model for decoration
    CModel box;			// a single box for level progression / end
	CModel shells;
	CModel bullets;
	CModel shotgun;
	CModel health;
	CModel bluekey;
	CModel redkey;
	CModel yellowkey;
	CModel movingwall; // used as a hittest damage trap
	CModel saw; // used as a hittest damage trap
	CModel testwall1; // detecting walls between enemy and player on x axis
	CModel testwall2; // detecting walls between enemy and player on z axis
	CModel headonstick;
	
	CModelList walls;   // list containing models
	CModelList Imps;   // list of Imp enemies
	CModelList impballs; // list of imp ranged projectiles
	CModelList ammos; // list of ammo packs
	CModelList healths; // list of health packs
	CModelList keys; // list of keys
	CModelList shots; // list of player shots
	CModelList guns; // contains player attached gun
	CModelList saws; // for level 2 trap
	CModelList staticmodels; // non animated models
	CModelList animatedmodels; // animated models list
	CModelList movingwalls; // for level 3

	CDoor door1;       // blue door
	CDoor door2;		// yellow door
	CDoor door3;		// red door

	// startscreen images
	CSprite startScreen;
	CSprite easybutton;
	CSprite normalbutton;
	CSprite hardbutton;
	CSprite level1button;
	CSprite level2button;
	CSprite bonusbutton;
	CSprite helpbutton;
	CSprite helpscreen;

	// gameover complted stats
	CSprite statscreen;
	CSprite rtnmenubtn;

	// hud images
	CSprite hud;
	CSprite face;
	CSprite bluekeyimg;
	CSprite yellowkeyimg;
	CSprite redkeyimg;
	CSprite reticle;
	
	// game world floor
	CFloor floor;

	// health indicator
	CHealthBar hbar;
	
	// Font
	CFont font;

	// sounds
	CSoundPlayer pistolshot;
	CSoundPlayer shotgunshot;
	CSoundPlayer playerpain;
	CSoundPlayer playerdeath;
	CSoundPlayer impballsound;
	CSoundPlayer imppain;
	CSoundPlayer impdeath;
	CSoundPlayer itempickup;
	CSoundPlayer keypickup;
	CSoundPlayer shotgunpickup;
	CSoundPlayer revattack;
	CSoundPlayer revdeath;
	CSoundPlayer dooropen;
	CSoundPlayer levelmusic;

   // -----   Add you member functions here ------
   
	void MovingwallControl();
	
	void PlayerControl(); 
   
	void ImpControl();

	void ImpShotsControl();

	void ShotsControl();

	void GameoverControl();

	void AmmoControl();

	void HealthControl();

	void KeysControl();

	void SawsControl();
   
	void DoorpuzzleControl();

	void CameraControl(CGraphics* g);
	
	
   // ---------------------Event Handling --------------------------

	// Game Loop Funtions
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnRender3D(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartLevel(int level);
	virtual void OnStartGame();
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
