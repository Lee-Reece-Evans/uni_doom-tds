#include "MyGame.h"
#include <fstream>

using namespace std;

CMyGame::CMyGame(void) {}

CMyGame::~CMyGame(void) {}

// --------  game initialisation --------
void CMyGame::OnInitialize()
{
	// Loading graphics and sound assets
	cout << "Loading assets" << endl;
	
	font.LoadDefault(); 
	
	// enable lighting
	Light.Enable();
	Light.SetAmbientLight(0.4, 0.4, 0.4);
	Light.SetDiffuseLight(0.6, 0.6, 0.6);

	// player model
	player.LoadModel("ss-soldier.md2", "ss-soldier.bmp"); 
	player.SetScale( 2.5f);

	// player weapon
	gun.LoadModel("ss-weapon.md2", "ss-weapon.bmp");
	gun.SetScale(2.5f);

	// shotgun model
	shotgun.LoadModel("shotgun.obj", "shotgun.bmp");
	shotgun.SetScale(2.5f);

	// Imp model
	Imp.LoadModel("Imp.md2","Imp.bmp");
	Imp.SetScale(2.2f);
	Imp.SetHealth(100);
	Imp.SetStatus(0);

	// imp fireball
	impball.LoadModel("impball.md2", "impball.bmp");
	impball.SetScale(1.0f);

	// ammo models
	shells.LoadModel("shellbox.obj", "shellbox.bmp"); 
	shells.SetScale(2.0f);

	bullets.LoadModel("pistolammo.obj", "pistolammo.bmp");
	bullets.SetScale(2.0f);

	// health model
	health.LoadModel("Medikit.obj", "Medikit.bmp");
	health.SetScale(2.0f);

	// key models
	bluekey.LoadModel("bluekey.obj", "bluekey.bmp"); bluekey.SetScale(1.0f);
	redkey.LoadModel("bluekey.obj", "redkey.bmp"); redkey.SetScale(1.0f);
	yellowkey.LoadModel("bluekey.obj", "yellowkey.bmp"); yellowkey.SetScale(1.0f);

	// sawblade model
	saw.LoadModel("sawblade.obj", "saw2.bmp");
	saw.SetScale(1020.0f);

	// head on stick model
	headonstick.LoadModel("headpole.obj", "headpole.bmp");
	headonstick.SetScale(2.7f);

	// twitching body
	twitcher.LoadModel("twitcher.md2", "twitcher.bmp");
	twitcher.SetScale(3.0f);
	
	// floor texture
	floor.LoadTexture("floor3.bmp");
	floor.SetTiling(true);
	
	// ---- start screen -----
	startScreen.LoadImage("startScreen.bmp");
	startScreen.SetPosition(Width/2.0f,Height/2.0f);

	// difficulty
	easybutton.LoadImage("easyNorm.bmp");
	easybutton.SetPosition(345.0f, 375.0f);

	normalbutton.LoadImage("normalPress.bmp");
	normalbutton.SetPosition(465.0f, 375.0f);

	hardbutton.LoadImage("hardNorm.bmp");
	hardbutton.SetPosition(585.0f, 375.0f);

	// levels
	level1button.LoadImage("level1Norm.bmp");
	level1button.SetPosition(460.0f, 270.0f);

	level2button.LoadImage("level2Norm.bmp");
	level2button.SetPosition(463.0f, 235.0f);

	bonusbutton.LoadImage("bonusNorm.bmp");
	bonusbutton.SetPosition(454.0f, 200.0f);

	// help button
	helpbutton.LoadImage("helpNorm.bmp");
	helpbutton.SetPosition(463.0f, 140.0f);

	// helpscreen
	helpscreen.LoadImage("helpscreen.bmp");
	helpscreen.SetPosition(Width / 2.0f, Height / 2.0f);

	// ---- hud ----
	hud.LoadImage("doomhud.bmp");
	hud.SetPosition(Width / 2.0f, hud.GetHeight() / 2.0f);

	// hud face
	face.LoadImage("doomface.bmp");
	face.SetPosition(452, 40);

	// hud keys
	bluekeyimg.LoadImage("bluekey.bmp");
	bluekeyimg.SetPosition(682, 75);
	yellowkeyimg.LoadImage("yellowkey.bmp");
	yellowkeyimg.SetPosition(682, 44);
	redkeyimg.LoadImage("redkey.bmp");
	redkeyimg.SetPosition(682, 13);

	// ----- stat screen ----
	statscreen.LoadImage("statscreen.bmp");
	statscreen.SetPosition(Width / 2.0f, Height / 2.0f);

	rtnmenubtn.LoadImage("mainmenubtn.bmp");
	rtnmenubtn.SetPosition(Width / 2.0f, Height / 2.0f - 55.0f);

	// player aiming/direction reticle
	reticle.LoadImage("reddot2.bmp");
	reticle.SetPosition(Width / 2.0f, Height / 2.0f + 50);

	// initialise some variables
	level=1;
	score = 0;
	difficulty = 1.0;
	pistolammo = 10;
	shotgunammo = 0;
	levelcompleted = false;
}

float rand(float range)
{
	return ((float)rand() / (float)RAND_MAX) * range * 2 - range;
}

CVector seek(CVector myPos, CVector myVel, CVector targetPos, float speed)
{
	// ---- Implement the SEEK behaviour ----
	CVector desiredVelocity, steering;

	desiredVelocity = targetPos - myPos;
	desiredVelocity = desiredVelocity.Normalized() * speed;
	steering = desiredVelocity - myVel;
	return steering;
}

CVector wander(CVector myPos, CVector myVel)
{
	// ---- Implement the WANDER behaviour ----
	CVector target, realTarget; 

	int wanderRadius = 20;
	int wanderDist = 200;

	target = target + CVector(rand(10), 0, rand(10));
	target = target.Normalized();
	realTarget = myPos + myVel.Normalized() * wanderDist + target * wanderRadius;

	return seek(myPos, myVel, realTarget, 150);
}


void CMyGame::OnStartLevel(int level)
{
	if (level == 1)
	{
		// --- clear model lists ----
		walls.clear();
		Imps.clear();
		impballs.clear();
		ammos.clear();
		healths.clear();
		keys.clear();
		shots.clear();
		guns.clear();
		saws.clear();
		movingwalls.clear();
		staticmodels.clear();
		animatedmodels.clear();

		// ammo
		pistolammo = 10 * difficulty;
		shotgunammo = 0;

		// set size of the game world
		floor.SetSize(12000, 12000);
		// move gameworld so that it starts at 0,0 and extends in positive xz-direction
		floor.SetPosition(0, 0);

		// play music
		levelmusic.Stop(); levelmusic.Clear();
		levelmusic.Play("level1music.wav", -1); levelmusic.SetVolume(30);

		// set box position, size and motion
		box.SetSize(100, 100, 100);  box.SetPosition(2500, 1200); box.SetToFloor(0);
		box.SetColor(CColor::Blue());

		// initalise pointer to gun model
		CModel* pgun = gun.clone();
		guns.push_back(pgun);

		// attach gun to player
		player.AttachModel(pgun);

		// player position
		player.SetPosition(-3650, 0, 0)/* -3650, 0, 0 */; player.SetDirection(90); player.SetRotation(90); player.SetStatus(0);

		// reset boolean
		hasbluekey = false;
		hasredkey = false;
		hasyellowkey = false;
		usingpistol = true;
		usingshotgun = false;
		collectedshotgun = false;
		levelcompleted = false;

		// --- level details ---
		fstream myfile;
		myfile.open("level1.txt", ios_base::in);

		int type, x, y, z, w, h, d, rot, stat;
		do
		{
			myfile >> type >> x >> y >> z >> w >> h >> d >> rot >> stat;

			if (type == 1) // walls
			{
				CModel* pWall = new CModel((float)x, (float)y, (float)z, (float)w, (float)h, (float)d);
				pWall->Rotate((float)rot); pWall->SetStatus(stat);
				pWall->SetColors(CColor::Black(), CColor::Red());
				walls.push_back(pWall);
			}
			if (type == 2) // imps
			{
				CModel* pEnemy = Imp.clone();
				pEnemy->SetPosition(x, y, z);
				pEnemy->SetSpeed(150);
				Imps.push_back(pEnemy);
			}
			if (type == 3) // pistol ammo
			{
				CModel* pPstAmmo = bullets.clone();
				pPstAmmo->SetStatus(stat);
				pPstAmmo->SetPosition(x, z);
				pPstAmmo->SetToFloor(0);
				ammos.push_back(pPstAmmo);
			}
			if (type == 4) // shotgun ammo
			{
				CModel* pShtAmmo = shells.clone();
				pShtAmmo->SetStatus(stat);
				pShtAmmo->SetPosition(x, z);
				pShtAmmo->SetToFloor(0);
				ammos.push_back(pShtAmmo);
			}
			if (type == 5) // health packs
			{
				CModel* pHealth = health.clone();
				pHealth->SetPosition(x, z);
				pHealth->SetToFloor(0);
				pHealth->SetRotation(rot);
				healths.push_back(pHealth);
			}
			if (type == 6) // saw blades
			{
				CModel* pSaw = saw.clone();
				pSaw->SetPosition(x, y, z); pSaw->SetOmega(0, 0, rot); pSaw->SetSpeed(stat);
				saws.push_back(pSaw);
			}
			if (type == 7) // head on stick
			{
				CModel* pModel = headonstick.clone();
				pModel->SetPosition(x, y, z);
				pModel->SetRotation(rot);
				staticmodels.push_back(pModel);
			}
			if (type == 8) // twitching body
			{
				CModel* pAnimatedmodel = twitcher.clone();
				pAnimatedmodel->SetPosition(x, y, z);
				pAnimatedmodel->SetRotation(rot);
				pAnimatedmodel->PlayAnimation(1, 3, 10, true);
				animatedmodels.push_back(pAnimatedmodel);
			}

		} while (!myfile.eof());

		myfile.close();

		// for use in testing if there is a wall between player and enemy
		testwall1 = CModel(100, 100, 100, 50, 50, 50, CColor::Yellow());
		testwall2 = CModel(100, 100, 100, 50, 50, 50, CColor::Blue());

		// moving wall trap, does damage if touches player
		movingwall = CModel(-3800, 150, 1650, 50, 300, 965, CColor::Red());
		movingwall.SetDirection(0);
		movingwall.SetSpeed(200);
		movingwall.SetAlpha(50);

		// ----  set up door ----
		// blue door
		door1.SetStatus(0);
		door1.Stop();
		door1.SetSize(600, 300, 75);
		door1.SetColor(CColor::Blue());
		door1.SetPosition(-2100, 300 / 2 - 10, -3400);
		door1.SetPositions(CVector(-2600, 300 / 2 - 10, -3400), CVector(-1600, 300 / 2 - 10, -3400));
		//yellow door
		door2.SetStatus(0);
		door2.Stop();
		door2.SetSize(75, 300, 700);
		door2.SetColor(CColor::Yellow());
		door2.SetPosition(-1800, 300 / 2 - 10, 1500);
		door2.SetPositions(CVector(-1800, 300 / 2 - 10, 2000), CVector(-1800, 300 / 2 - 10, 1500));
		// red door
		door3.SetStatus(0);
		door3.Stop();
		door3.SetSize(600, 300, 75);
		door3.SetColor(CColor::Red());
		door3.SetPosition(3700, 300 / 2 - 10, -1300);
		door3.SetPositions(CVector(3200, 300 / 2 - 10, -1300), CVector(3700, 300 / 2 - 10, -1300));

		// ---- setup keycards ----
		//blue key
		CModel* pBKey = bluekey.clone();
		pBKey->SetPosition(-3600, 50, -3200); pBKey->SetOmega(100); pBKey->SetStatus(1);
		keys.push_back(pBKey);
		// yellow key
		CModel* pYKey = yellowkey.clone();
		pYKey->SetPosition(-3600, 50, 3600); pYKey->SetOmega(100);  pYKey->SetStatus(3);
		keys.push_back(pYKey);
		// red key
		CModel* pRKey = redkey.clone();
		pRKey->SetPosition(3600, 50, -3600); pRKey->SetOmega(100); pRKey->SetStatus(2);
		keys.push_back(pRKey);

		// shotgun placement
		CModel* pShotgun = shotgun.clone();
		pShotgun->SetStatus(3);
		pShotgun->SetPosition(-1200, -2400);
		pShotgun->SetToFloor(0);
		pShotgun->Rotate(90);
		ammos.push_back(pShotgun);
	}
	
	if (level == 2)
	{
		// --- clear model lists ----
		walls.clear();
		Imps.clear();
		impballs.clear();
		ammos.clear();
		healths.clear();
		keys.clear();
		shots.clear();
		guns.clear();
		saws.clear();
		movingwalls.clear();
		staticmodels.clear();
		animatedmodels.clear();

		// ammo 
		if (levelcompleted) // if progressing from previous level- keep ammo
		{
			pistolammo = pistolammo;
			shotgunammo = shotgunammo;
		}
		else
		{
			pistolammo = 10 * difficulty;
			shotgunammo = 0;
		}

		// set size of the game world
		floor.SetSize(11000, 15000);
		// move gameworld so that it starts at 0,0 and extends in positive xz-direction
		floor.SetPosition(1000, 1000);

		// play music
		levelmusic.Stop(); levelmusic.Clear();
		levelmusic.Play("level2music.wav", -1); levelmusic.SetVolume(30);

		// set box position, size and motion
		box.SetSize(100, 100, 100);  box.SetPosition(-1600, 7000); box.SetToFloor(0);
		box.SetColor(CColor::Blue());

		// initalise pointer to gun model
		CModel* pgun = gun.clone();
		guns.push_back(pgun);

		// attach gun to player
		player.AttachModel(pgun);

		// player position
		player.SetPosition(-1000, 0, 0); player.SetDirection(0); player.SetRotation(0); player.SetStatus(0);

		// reset boolean
		hasbluekey = false;
		hasredkey = false;
		hasyellowkey = false;
		usingpistol = true;
		usingshotgun = false;
		if (levelcompleted && collectedshotgun) collectedshotgun; // keep shotgun if progressing from previous level
		else collectedshotgun = false;
		levelcompleted = false;

		// --- level details ---
		fstream myfile;
		myfile.open("level2.txt", ios_base::in);

		int type, x, y, z, w, h, d, rot, stat;
		do
		{
			myfile >> type >> x >> y >> z >> w >> h >> d >> rot >> stat;

			if (type == 1) // walls
			{
				CModel* pWall = new CModel((float)x, (float)y, (float)z, (float)w, (float)h, (float)d);
				pWall->SetRotation(rot); pWall->SetStatus(stat);
				pWall->SetColors(CColor::Black(), CColor::Red());
				walls.push_back(pWall);
			}
			if (type == 2) // imps
			{
				CModel* pEnemy = Imp.clone();
				pEnemy->SetPosition(x, y, z);
				pEnemy->SetSpeed(150);
				Imps.push_back(pEnemy);
			}
			if (type == 3) // pistol ammo
			{
				CModel* pPstAmmo = bullets.clone();
				pPstAmmo->SetStatus(stat);
				pPstAmmo->SetPosition(x, z);
				pPstAmmo->SetToFloor(0);
				ammos.push_back(pPstAmmo);
			}
			if (type == 4) // shotgun ammo
			{
				CModel* pShtAmmo = shells.clone();
				pShtAmmo->SetStatus(stat);
				pShtAmmo->SetPosition(x, z);
				pShtAmmo->SetToFloor(0);
				ammos.push_back(pShtAmmo);
			}
			if (type == 5) // health packs
			{
				CModel* pHealth = health.clone();
				pHealth->SetPosition(x, z);
				pHealth->SetToFloor(0);
				pHealth->SetRotation(rot);
				healths.push_back(pHealth);
			}
			if (type == 6) // saw blades
			{
				CModel* pSaw = saw.clone();
				pSaw->SetPosition(x, y, z); pSaw->SetOmega(0, 0, rot); pSaw->SetSpeed(stat);
				saws.push_back(pSaw);
			}
			if (type == 7) // head on stick
			{
				CModel* pModel = headonstick.clone();
				pModel->SetPosition(x, y, z);
				pModel->SetRotation(rot);
				staticmodels.push_back(pModel);
			}
			if (type == 8) // twitching body
			{
				CModel* pAnimatedmodel = twitcher.clone();
				pAnimatedmodel->SetPosition(x, y, z);
				pAnimatedmodel->SetRotation(rot);
				pAnimatedmodel->PlayAnimation(1, 3, 10, true);
				animatedmodels.push_back(pAnimatedmodel);
			}
		} while (!myfile.eof());

		myfile.close();

		// for use in testing if there is a wall between player and enemy
		testwall1 = CModel(100, 100, 100, 50, 50, 50, CColor::Yellow());
		testwall2 = CModel(100, 100, 100, 50, 50, 50, CColor::Blue());

		// moving wall trap, does damage if touches player
		movingwall = CModel(-400, 150, 4140, 50, 300, 965, CColor::Red());
		movingwall.SetDirection(0);
		movingwall.SetSpeed(200);
		movingwall.SetAlpha(50);

		// ----  set up door ----
		// blue door
		door1.SetStatus(0);
		door1.Stop();
		door1.SetSize(125, 300, 550);
		door1.SetColor(CColor::Blue());
		door1.SetPosition(900, 300 / 2 - 10, -1900);
		door1.SetPositions(CVector(900, 300 / 2 - 10, -1400), CVector(900, 300 / 2 - 10, -1900));
		//yellow door
		door2.SetStatus(0);
		door2.Stop();
		door2.SetSize(125, 300, 740);
		door2.SetColor(CColor::Yellow());
		door2.SetPosition(1075, 300 / 2 - 10, 1240);
		door2.SetPositions(CVector(1075, 300 / 2 - 10, 1900), CVector(1075, 300 / 2 - 10, 1240));
		// red door
		door3.SetStatus(0);
		door3.Stop();
		door3.SetSize(400, 300, 125);
		door3.SetColor(CColor::Red());
		door3.SetPosition(125, 300 / 2 - 10, 4700);
		door3.SetPositions(CVector(-225, 300 / 2 - 10, 4700), CVector(125, 300 / 2 - 10, 4700));

		// ---- setup keycards ----
		//blue key
		CModel* pBKey = bluekey.clone();
		pBKey->SetPosition(3290, 50, 400); pBKey->SetOmega(100); pBKey->SetStatus(1);
		keys.push_back(pBKey);
		// yellow key
		CModel* pYKey = yellowkey.clone();
		pYKey->SetPosition(4860, 50, 6740); pYKey->SetOmega(100);  pYKey->SetStatus(3);
		keys.push_back(pYKey);
		// red key
		CModel* pRKey = redkey.clone();
		pRKey->SetPosition(2025, 50, 1830); pRKey->SetOmega(100); pRKey->SetStatus(2);
		keys.push_back(pRKey);

		// shotgun placement
		CModel* pShotgun = shotgun.clone();
		pShotgun->SetStatus(3);
		pShotgun->SetRotation(90);
		pShotgun->SetPosition(-950, 3250);
		pShotgun->SetToFloor(0);
		ammos.push_back(pShotgun);
	}


	if (level == 3)
	{
		// --- clear model lists ----
		walls.clear();
		Imps.clear();
		impballs.clear();
		ammos.clear();
		healths.clear();
		keys.clear();
		shots.clear();
		guns.clear();
		saws.clear();
		movingwalls.clear();
		staticmodels.clear();
		animatedmodels.clear();

		// initial ammo
		pistolammo = 50 * difficulty;
		shotgunammo = 0;

		floor.SetSize(4000, 4000);
		floor.SetPosition(0, 0);

		// set box position, size and motion
		box.SetSize(100, 100, 100);  box.SetPosition(-1800, -760); box.SetToFloor(0);
		box.SetColor(CColor::Blue());

		// initalise pointer to gun model
		CModel* pgun = gun.clone();
		guns.push_back(pgun);

		// attach gun to player
		player.AttachModel(pgun);

		// player position
		player.SetPosition(-1000, 0, 0); player.SetDirection(-90); player.SetRotation(-90); player.SetStatus(0);

		// reset boolean
		puzzleactive = false;
		hasbluekey = false;
		hasredkey = false;
		hasyellowkey = false;
		usingpistol = true;
		usingshotgun = false;
		collectedshotgun = false;
		levelcompleted = false;

		// --- level details ---
		fstream myfile;
		myfile.open("level3.txt", ios_base::in);

		int type, x, y, z, w, h, d, rot, speed;
		do
		{
			myfile >> type >> x >> y >> z >> w >> h >> d >> rot >> speed;

			if (type == 1) // walls
			{
				CModel* pWall = new CModel((float)x, (float)y, (float)z, (float)w, (float)h, (float)d);
				pWall->Rotate((float)rot);
				pWall->SetColors(CColor::Black(), CColor::Red());
				walls.push_back(pWall);
			}
			if (type == 2) // blue moving walls
			{
				CModel* pBluewall = new CModel((float)x, (float)y, (float)z, (float)w, (float)h, (float)d);
				pBluewall->Rotate((float)rot); pBluewall->SetDirection(pBluewall->GetRotation());
				pBluewall->SetSpeed(speed);
				pBluewall->SetStatus(1);
				pBluewall->SetAlpha(50);
				movingwalls.push_back(pBluewall);
			}
			if (type == 3) // blue moving walls
			{
				CModel* pGreenwall = new CModel((float)x, (float)y, (float)z, (float)w, (float)h, (float)d);
				pGreenwall->Rotate((float)rot); pGreenwall->SetDirection(pGreenwall->GetRotation());
				pGreenwall->SetSpeed(speed);
				pGreenwall->SetStatus(2);
				pGreenwall->SetColor(CColor::Green());
				pGreenwall->SetAlpha(50);
				movingwalls.push_back(pGreenwall);
			}
		} while (!myfile.eof());

		myfile.close();

		// moving wall
		movingwall = CModel(-1085, 150, -825, 50, 300, 750, CColor::Red());  // Don't change this.
		movingwall.SetRotation(90); movingwall.SetDirection(movingwall.GetRotation());
		movingwall.SetSpeed(0);
		movingwall.SetAlpha(0);

		// for use in testing if there is a wall between player and enemy
		testwall1 = CModel(100, 100, 100, 50, 50, 50, CColor::Yellow());
		testwall2 = CModel(100, 100, 100, 50, 50, 50, CColor::Blue());

		// blue door
		door1.SetStatus(0);
		door1.Stop();
		door1.SetSize(75, 300, 550);
		door1.SetColor(CColor::White());
		door1.SetPosition(-650, 300 / 2 - 10, 700);
		door1.SetPositions(CVector(-650, 300 / 2 - 10, 300), CVector(-650, 300 / 2 - 10, 700));
		// yellow door
		door2.SetStatus(0);
		door2.Stop();
		door2.SetSize(75, 300, 550);
		door2.SetColor(CColor::White());
		door2.SetPosition(-650, -1000, 700);
		door2.SetPositions(CVector(-650, 300 / 2 - 10, 300), CVector(-650, 300 / 2 - 10, 700));
		//red door
		door3.SetStatus(0);
		door3.Stop();
		door3.SetSize(75, 300, 550);
		door3.SetColor(CColor::White());
		door3.SetPosition(-650, -1000, 700);
		door3.SetPositions(CVector(-650, 300 / 2 - 10, 300), CVector(-650, 300 / 2 - 10, 700));
	}
}

// Game Logic in the OnUpdate function called every frame

void CMyGame::OnUpdate() 
{
	if (IsMenuMode() || IsGameOver()) return;
	
	long t =  GetTime();
	
	// --- updating models ----
	player.Update(t);
	guns.Update(t);
	Imps.Update(t);
	impballs.Update(t);
	testwall1.Update(t);
	testwall2.Update(t);
	walls.Update(t);
	shots.Update(t);
	box.Update(t);
	movingwall.Update(t);
	movingwalls.Update(t);
	saws.Update(t);
	ammos.Update(t);
	healths.Update(t);
	keys.Update(t);
	reticle.Update(t);
	animatedmodels.Update(t);
	
	door1.Update(t);
	door2.Update(t);
	door3.Update(t);
	
	PlayerControl();
	ImpControl();
	ImpShotsControl();
	ShotsControl();
	GameoverControl();
	AmmoControl();
	HealthControl();
	KeysControl();
	SawsControl();
	MovingwallControl();

	if (level == 3) DoorpuzzleControl();
}

void CMyGame::PlayerControl()
{
	if (player.GetStatus() != 1) // not dead
	{
		if (IsLButtonDown() && usingpistol && pistolammo > 0) // shoot pistol
		{

			player.SetSpeed(0);
			player.PlayAnimation(8, 12, 10, true);
			if (player.AnimationFinished())
			{
				pistolammo -= 1;

				pistolshot.Play("pistolshot.wav"); pistolshot.SetVolume(20);
				// create a new shot dynamically as a short line
				CModel* pShot = new CLine(player.GetX() + 30.0f, player.GetY() + 50.0f, player.GetZ(), 50, CColor::White());
				pShot->SetDirection(player.GetRotation());
				pShot->SetRotation(player.GetRotation());
				pShot->SetSpeed(3000);
				pShot->Move(120);
				shots.push_back(pShot);
			}
		}
		else if (IsLButtonDown() && usingshotgun && shotgunammo > 0) // shoot shotgun
		{
			player.SetSpeed(0);
			player.PlayAnimation(8, 12, 10, false);
			if (player.AnimationFinished())
			{
				shotgunammo -= 1;

				shotgunshot.Play("shotgunshot.wav"); shotgunshot.SetVolume(25);
				// create a new shot dynamically as a short line
				for (int i = -2; i <= 2; i += 2)
				{
					CModel* pShot = new CLine(player.GetX() + 30.0f, player.GetY() + 50.0f, player.GetZ(), 50, CColor::White());
					pShot->SetDirection(player.GetRotation() + i);
					pShot->SetRotation(player.GetRotation());
					pShot->SetSpeed(3000);
					pShot->Move(120);
					shots.push_back(pShot);
				}
			}
		}
		else if (IsKeyDown(SDLK_w)) // movement
		{
			bool move = true;

			// checking whether the player collides with any of the walls 
			for (CModel* pWall : walls)
			{
				// don't allow to move if there is a collision
				if (player.HitTestFront(pWall)) move = false;
			}

			// check if player collides with the doors
			if (player.HitTestFront(&door1) || player.HitTestFront(&door2) || player.HitTestFront(&door3)) move = false;

			for (CModel* pImp : Imps)
			{
				if (player.HitTestFront(pImp)) move = false;
			}

			// if no collision, move 
			if (move)
			{
				player.SetSpeed(500);
				player.PlayAnimation(1, 7, 10, true);
			}
			else
			{
				player.SetSpeed(0);
				player.PlayAnimation(0, 0, 10, true);
			}
		}
		else
		{
			player.SetSpeed(0);
			player.PlayAnimation(0, 0, 10, true);
		}
	}
}

void CMyGame::ImpControl()
{
	// ----- A.I. enemies (Imps) control -------
	for (CModel* pImp : Imps)
	{
		for (CModel* pWall : walls)
		{
			// is Imp hitting a wall or door?
			if (pImp->HitTestFront(pWall) || pImp->HitTestFront(&door1) || pImp->HitTestFront(&door2) || pImp->HitTestFront(&door3))
			{
				// change direction and rotation of hitting a wall
				pImp->SetRotation(pImp->GetRotation() + 90);
				pImp->SetDirection(pImp->GetRotation());
				break;
			}
		}
		// has Imp caught the player?
		if (pImp->HitTest(&player) && pImp->GetHealth() > 0)
		{
			//attack status
			pImp->SetStatus(3);
			pImp->PlayAnimation(6, 10, 10, true);
			pImp->SetSpeed(0);
		}
		// has imp attacked and hit player?
		if (pImp->GetStatus() == 3 && pImp->AnimationFinished() && pImp->HitTest(&player))
		{
			playerpain.Clear();
			playerpain.Play("playerpain.wav"); playerpain.SetVolume(15);
			pImp->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
			hbar.SetHealth(hbar.GetHealth() - 10);
		}
		// re-activate walk anim
		if (pImp->GetStatus() == 1 && pImp->AnimationFinished() || pImp->GetStatus() == 3 && pImp->AnimationFinished() && !pImp->HitTest(&player))
		{
			//idle status
			pImp->SetStatus(0);
		}

		// ---- wandering & "seek" behaviour with enemy to player wall detection to avoid enemies following player through a wall ----
		if (pImp->GetStatus() == 0)
		{
			CVector steering, vel;
			// imps velocity vector
			vel = CVector(pImp->GetXVelocity(), pImp->GetYVelocity(), pImp->GetZVelocity());

			//calculate distance between enemy and player
			CVector d = player.GetPositionV() - pImp->GetPositionV();
			float distance = sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
			// x and z distance
			float x = sqrt(d.x *d.x);
			float z = sqrt(d.z * d.z);

			// enemy steering force
			steering = wander(pImp->GetPositionV(), vel.Normalized());

			// target distance
			if (distance < 720)
			{
				// ---- wall detection method ----
				// set at mid point 
				testwall1.SetPosition(d.x * 0.5 + pImp->GetX(), d.y * 0.5 + pImp->GetY() + 100, d.z * 0.5 + pImp->GetZ());
				testwall2.SetPosition(d.x * 0.5 + pImp->GetX(), d.y * 0.5 + pImp->GetY() + 100, d.z * 0.5 + pImp->GetZ());
				// set size to the displacement 
				testwall1.SetSize(x, 30, 30);
				testwall2.SetSize(30, 30, z);
				// set direction of detection to player position from enemy position
				testwall1.SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
				// rotate z axis to face player
				testwall2.SetRotation(testwall1.GetRotation() + 90);
				testwall2.SetDirection(testwall2.GetRotation());

				// ---- applying behaviour ----
				bool iswall = false;

				for (CModel* wall : walls)
				{
					// if wall is detected between player and enemy - enemy keeps wandering
					if (wall->GetRotation() == 0) // for use with horizonal x coordinate for width and z for depth
					{
						if (testwall1.GetFront() >= wall->GetBack() && testwall1.GetBack() <= wall->GetFront()
							&& testwall1.GetLeft() <= wall->GetRight() && testwall1.GetRight() >= wall->GetLeft()) // using size functions in model.h file
						{
							pImp->SetVelocityV(steering);
							pImp->SetRotation(pImp->GetDirection());
							iswall = true;
							//cout << "there is a wall." << endl;
							break;
						}
					}
					else if (wall->GetRotation() == 180) // switch for use with verticle walls of z coordinate with width and x with depth - 180deg for identification
					{
						if (testwall2.GetRight() >= wall->GetLeft() && testwall2.GetLeft() <= wall->GetRight()
							&& testwall2.GetBack() <= wall->GetFront() && testwall2.GetFront() >= wall->GetBack()) // using size functions in model.h file
						{
							pImp->SetVelocityV(steering);
							pImp->SetRotation(pImp->GetDirection());
							iswall = true;
							//cout << "there is a wall." << endl;
							break;
						}
					}
				}
				if (!iswall) // no wall detected
				{
					if (distance > 400)
					{
						// ranged attack
						pImp->SetSpeed(1);
						pImp->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
						pImp->PlayAnimation(6, 10, 8, true);

						if (pImp->AnimationFinished())
						{
							impballsound.Play("impball.wav"); impballsound.SetVolume(10);
							CModel* pBall = impball.clone();
							pBall->SetPosition(pImp->GetX(), pImp->GetY() + 50.0f, pImp->GetZ());
							pBall->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
							pBall->SetSpeed(750);
							pBall->Move(90);
							impballs.push_back(pBall);

							pBall->PlayAnimation(1, 5, 10, true);
						}
					}
					else
					{
						// enemy seeks player for close attack (status 3)
						pImp->SetDirectionAndRotationToPoint(player.GetX(), player.GetZ());
						pImp->SetSpeed(375);
						//cout << "no wall." << endl;
					}
				}
			}
			else
			{
				pImp->SetVelocityV(steering);
				pImp->SetRotation(pImp->GetDirection());
			}
			// ---- walk animations ----
			if (pImp->GetSpeed() > 1)
			{
				pImp->PlayAnimation(2, 5, 5, true);
			}
			else if (pImp->GetSpeed() == 0) pImp->PlayAnimation(0, 1, 5, true);
		}
	}
}

void CMyGame::ImpShotsControl()
{
	for (CModel* pBall : impballs)
	{
		// hittest imp fireball with player
		if (pBall->HitTest(&player))
		{
			playerpain.Clear();
			playerpain.Play("playerpain.wav"); playerpain.SetVolume(15);
			hbar.SetHealth(hbar.GetHealth() - 5);
			pBall->Delete();
		}
		// hitest with doors
		if (pBall->HitTest(&door1) || pBall->HitTest(&door2) || pBall->HitTest(&door3))
		{
			pBall->Delete();
		}
		// hittest imp fireball with walls and doors
		for (CModel* wall : walls)
		{
			if (pBall->HitTest(wall))
			{
				pBall->Delete();
			}
		}
	}
	impballs.remove_if(deleted);
}

void CMyGame::ShotsControl()
{
	// collision detection between shots and enemies
	for (CModel* pShot : shots)
	{
		// shots collision with imps
		for (CModel* pImp : Imps)
		{
			if (pShot->HitTest(pImp) && pImp->GetStatus() != 2)
			{
				// pain anim
				if (pImp->GetHealth() > 0)
				{
					//pain status
					pImp->SetStatus(1);
					pImp->SetSpeed(0);
					pImp->SetHealth(pImp->GetHealth() - 20);
					pShot->Delete();
					imppain.Clear();
					imppain.Play("imppain.wav"); imppain.SetVolume(15);
					pImp->PlayAnimation(11, 13, 5, false);
					score += 10;
				}
				//deathanim
				if (pImp->GetHealth() <= 0)
				{
					//death status
					pImp->SetStatus(2);
					pImp->Delete();
					imppain.Clear();
					impdeath.Play("impdeath.wav"); impdeath.SetVolume(10);
					pImp->PlayAnimation(14, 20, 10, false);
					pImp->Die(600);
					score += 50;
				}
			}
		}
		// shots collision with doors
		if (pShot->HitTest(&door1) || pShot->HitTest(&door2) || pShot->HitTest(&door3))
		{
			pShot->Delete();
		}
		// shots collision with walls
		for (CModel* wall : walls)
		{
			if (pShot->HitTest(wall) || pShot->HitTest(&door1) || pShot->HitTest(&door2) || pShot->HitTest(&door3))
			{
				pShot->Delete();
			}
		}
	}
	shots.remove_if(deleted);
	Imps.remove_if(deleted);
}

void CMyGame::GameoverControl()
{
	if (hbar.GetHealth() <= 0 && player.GetStatus() == 0)
	{
		player.SetStatus(1); // dead
		player.PlayAnimation(16, 20, 10, false);
		playerdeath.Play("playerdeath.wav"); playerdeath.SetVolume(15);
	}
	if (player.GetStatus() == 1 && player.AnimationFinished()) GameOver(); // play death anim before gameover
	
	// level progression
	if (player.HitTest(&box) && level == 1) { levelcompleted = true; level = 2; OnStartLevel(2); }
	else if (player.HitTest(&box) && level == 2) { levelcompleted = true; GameOver(); }
	else if (player.HitTest(&box) && level == 3) { levelcompleted = true; GameOver(); }
}

void CMyGame::AmmoControl()
{
	for (CModel* pAmmo : ammos)
	{
		if (player.HitTest(pAmmo))
		{
			if (pAmmo->GetStatus() == 1 && pistolammo < 200)
			{
				// play pickup item sound
				itempickup.Play("itempickup.wav"); itempickup.SetVolume(100);
				pistolammo += 10 * difficulty;
				if (pistolammo > 200) pistolammo = 200;
				pAmmo->Delete();
			}
			if (pAmmo->GetStatus() == 2 && shotgunammo < 50)
			{
				// play pickup item sound
				itempickup.Play("itempickup.wav"); itempickup.SetVolume(100);
				shotgunammo += 4 * difficulty;
				if (shotgunammo > 50) shotgunammo = 50;
				pAmmo->Delete();
			}
			if (pAmmo->GetStatus() == 3)
			{
				// play pickup item sound
				shotgunpickup.Play("shotgunpickup.wav");
				collectedshotgun = true;
				shotgunammo += 4 * difficulty;
				pAmmo->Delete();
			}
		}
	}
	ammos.remove_if(deleted);
}

void CMyGame::HealthControl()
{
	for (CModel* pHealth : healths)
	{
		if (player.HitTest(pHealth) && hbar.GetHealth() < 100)
		{
			// play pickup item sound
			itempickup.Play("itempickup.wav"); itempickup.SetVolume(100);
			// heal player, if overhealed reset to 100
			hbar.SetHealth(hbar.GetHealth() + 20.0 * difficulty);
			if (hbar.GetHealth() > 100) hbar.SetHealth(100);
			pHealth->Delete();
		}
	}
	healths.remove_if(deleted);
}

void CMyGame::KeysControl()
{
	for (CModel* pKey : keys)
	{
		if (player.HitTest(pKey))
		{
			// play sound
			keypickup.Play("keypickup.wav");
			// player picking up keys
			if (pKey->GetStatus() == 1) hasbluekey = true;
			else if (pKey->GetStatus() == 2) hasredkey = true;
			else hasyellowkey = true;
			pKey->Delete();
		}
	}
	keys.remove_if(deleted);
}

void CMyGame::SawsControl()
{
	for (CModel* pSaw : saws)
	{
		// hittest with player
		if (pSaw->HitTest(&player))
		{
			hbar.SetHealth(hbar.GetHealth() - 1);
		}

		// movement of saws
		if (pSaw->GetX() >= 4850)
		{
			pSaw->SetDirection(pSaw->GetDirection() + 180);
			pSaw->SetOmega(0, 0, 250);
		}
		if (pSaw->GetX() <= 4350)
		{
			pSaw->SetDirection(pSaw->GetDirection() + 180);
			pSaw->SetOmega(0, 0, -250);
		}
	}
}

void CMyGame::MovingwallControl()
{
	for (CModel* pWall : walls)
	{
		//move wall in opposite direction
		if (movingwall.HitTestFront(pWall) || movingwall.HitTestBack(pWall))
		{
			// change direction of hitting a wall
			movingwall.SetDirection(movingwall.GetDirection() + 180);
			break;
		}
	}
	// wall trap if player touches, hp is lost
	if (movingwall.HitTest(&player))
	{
		if (level != 3) hbar.SetHealth(hbar.GetHealth() - 2);
		else { if (puzzleactive) hbar.SetHealth(hbar.GetHealth() - hbar.GetHealth()); }
	}

	for (CModel* pmovingwall : movingwalls)
	{
		for (CModel* pWall : walls)
		{
			//move walls in opposite direction
			if (pmovingwall->HitTestFront(pWall) || pmovingwall->HitTestBack(pWall))
			{
				// change direction of hitting a wall
				pmovingwall->SetDirection(pmovingwall->GetDirection() + 180);
				break;
			}
		}
		// move player to start room if hit will blue walls
		if (player.HitTest(pmovingwall) && pmovingwall->GetStatus() == 1)
		{
			player.SetPosition(-400, 680);
		}
		// spawn imps if touch green walls
		if (player.HitTest(pmovingwall) && pmovingwall->GetStatus() == 2)
		{
			CModel* pEnemy = Imp.clone();
			pEnemy->SetPosition(-1400, 0, -1450);
			pEnemy->SetSpeed(150);
			Imps.push_back(pEnemy);

			pmovingwall->Delete();
		}
	}
	movingwalls.remove_if(deleted);
}

void CMyGame::DoorpuzzleControl()
{
	if (player.HitTestFront(&door1) && !puzzleactive)
	{
		// stop function
		puzzleactive = true;

		//blue key
		CModel* pBKey = bluekey.clone();
		pBKey->SetOmega(100); pBKey->SetStatus(1);
		keys.push_back(pBKey);
		// red key
		CModel* pRKey = redkey.clone();
		pRKey->SetOmega(100); pRKey->SetStatus(2);
		keys.push_back(pRKey);
		// yellow key
		CModel* pYKey = yellowkey.clone();
		pYKey->SetOmega(100);  pYKey->SetStatus(3);
		keys.push_back(pYKey);

		// initialise trap wall
		movingwall.SetAlpha(50);
		movingwall.SetSpeed(200);

		// create radom number between 1 and 3
		int randcolour = rand() % 3;

		// set random door
		if (randcolour == 1) door1.SetColor(CColor::Blue());
		else if (randcolour == 2) { door1.SetY(-1000); door2.SetY(300 / 2 - 10); door2.SetColor(CColor::Yellow()); }
		else { door1.SetY(-1000); door3.SetY(300 / 2 - 10); door3.SetColor(CColor::Red());}

		// place keys in random locations
		for (CModel* key : keys)
		{
			if (randcolour == 1)
			{
				if (key->GetStatus() == 1) key->SetPosition(-1155, 1720);
				if (key->GetStatus() == 2) key->SetPosition(-1420, 1720);
				if (key->GetStatus() == 3) key->SetPosition(-845, 1720);
			}
			else if (randcolour == 2)
			{
				if (key->GetStatus() == 1) key->SetPosition(-845, 1720);
				if (key->GetStatus() == 2) key->SetPosition(-1155, 1720);
				if (key->GetStatus() == 3) key->SetPosition(-1420, 1720);
			}
			else
			{
				if (key->GetStatus() == 1) key->SetPosition(-845, 1720);
				if (key->GetStatus() == 2) key->SetPosition(-1420, 1720);
				if (key->GetStatus() == 3) key->SetPosition(-1155, 1720);
			}
		}
	}
}

//-----------------  Draw 2D overlay ----------------------
void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())
	{
		if (helpscreenactive)
		{
			helpscreen.Draw(g);
		}
		else
		{
			startScreen.Draw(g);
			easybutton.Draw(g);
			normalbutton.Draw(g);
			hardbutton.Draw(g);
			level1button.Draw(g);
			level2button.Draw(g);
			bonusbutton.Draw(g);
			helpbutton.Draw(g);
		}
		return;
	}

	// draw hud
	hud.Draw(g);
	// ---- draw hud details ----
	// face
	face.Draw(g);
	if (hbar.GetHealth() > 70 && hbar.GetStatus() != 1) { face.LoadImage("doomface.bmp"); hbar.SetStatus(1); }
	else if (hbar.GetHealth() <= 70 && hbar.GetHealth() >= 40 && hbar.GetStatus() != 2) { face.LoadImage("doomfacehurt.bmp"); hbar.SetStatus(2); }
	else if (hbar.GetHealth() < 40 && hbar.GetHealth() > 0 && hbar.GetStatus() != 3) { face.LoadImage("doomfacehurtbad.bmp"); hbar.SetStatus(3); }
	else if (hbar.GetHealth() <= 0 && hbar.GetStatus() != 4) { face.LoadImage("doomfacedead.bmp"); hbar.SetStatus(4); }

	// ammo count
	if (usingpistol) { font.SetColor(CColor::Red()); if (pistolammo > 9) font.DrawNumber(40, 40, pistolammo); else font.DrawNumber(50, 40, pistolammo); }
	else { font.SetColor(CColor::Red()); if (shotgunammo > 9) font.DrawNumber(40, 40, shotgunammo); else font.DrawNumber(50, 40, shotgunammo); }

	font.SetColor(CColor::Yellow()); font.SetSize(18); font.DrawNumber(790, 45, pistolammo);
	font.SetColor(CColor::Yellow()); font.SetSize(18); font.DrawNumber(850, 45, 200);

	font.SetColor(CColor::Yellow()); font.SetSize(18); font.DrawNumber(790, 25, shotgunammo);
	font.SetColor(CColor::Yellow()); font.SetSize(18); font.DrawNumber(850, 25, 50);

	// arms
	font.SetColor(CColor::Yellow()); font.SetSize(20); font.DrawNumber(322, 46, 2);
	if (collectedshotgun) { font.SetColor(CColor::Yellow()); font.SetSize(20); font.DrawNumber(362, 46, 3); }
	else { font.SetColor(CColor::Gray()); font.SetSize(20); font.DrawNumber(362, 46, 3); }

	// keys
	if (hasbluekey) bluekeyimg.Draw(g);
	if (hasyellowkey) yellowkeyimg.Draw(g);
	if (hasredkey) redkeyimg.Draw(g);

	// draw player aiming reticle
	reticle.Draw(g);

	// draw health
	hbar.Draw(g);

	// draw level text
	font.SetColor(CColor::Green()); font.SetSize(32); font.DrawText(520, 50, "LEVEL");
	font.DrawNumber(620, 50, level);

	// draw score
	if (score < 100) { font.SetColor(CColor::White()); font.DrawNumber(560, 15, score); }
	else if (score > 100 && score < 1000) { font.SetColor(CColor::White()); font.DrawNumber(550, 15, score); }
	else { font.SetColor(CColor::White()); font.DrawNumber(540, 15, score); }

	// inform player to how to open door
	if (player.HitTest(&door1))
	{
		font.SetColor(CColor::Blue());
		if (door1.GetStatus() == 1);
		else if (!hasbluekey) font.DrawText(Width / 3, Height - 180, "FIND BLUE KEY TO OPEN DOOR");
		else font.DrawText(Width / 3, Height - 180, "PRESS E TO OPEN");
	}
	if (player.HitTest(&door2))
	{
		font.SetColor(CColor::Yellow());
		if (door2.GetStatus() == 1);
		else if (!hasyellowkey) font.DrawText(Width / 3, Height - 180, "FIND YELLOW KEY TO OPEN DOOR");
		else font.DrawText(Width / 3, Height - 180, "PRESS E TO OPEN");
	}
	if (player.HitTest(&door3))
	{
		font.SetColor(CColor::Red());
		if (door3.GetStatus() == 1);
		else if (!hasredkey) font.DrawText(Width / 3, Height - 180, "FIND RED KEY TO OPEN DOOR");
		else font.DrawText(Width / 3, Height - 180, "PRESS E TO OPEN");
	}

	// draw game paused text
	if (IsPaused())
	{
		font.SetSize(48); font.SetColor(CColor::Yellow()); font.DrawText(Width / 3.0f, Height / 2.0f, "GAME PAUSED");
	}

	// draw GAME OVER if game over
	if (IsGameOver() && !levelcompleted)
	{
		font.SetSize(48); font.SetColor(CColor::Red()); font.DrawText(Width / 3.0f, Height / 2.0f, "GAME OVER");
		font.SetSize(40); font.SetColor(CColor::Yellow()); font.DrawText(Width / 3.0f -75.0f, Height / 2.0f -50.0f, "PRESS F2 TO RESTART");
	}
	else if (IsGameOver() && levelcompleted)
	{
		statscreen.Draw(g);
		rtnmenubtn.Draw(g);
	}
}
	


// ----------------   Draw 3D world -------------------------
void CMyGame::OnRender3D(CGraphics* g)
{

	CameraControl(g);

	// ------- Draw your 3D Models here ----------
	
	floor.Draw(g);
	//floor.Draw(true); // show floor with grid

	box.Draw(g);
	walls.Draw(g);
	player.Draw(g);
	guns.Draw(g);
	Imps.Draw(g);
	impballs.Draw(g);
	shots.Draw(g);
	//testwall1.Draw(g);
	//testwall2.Draw(g);
	door1.Draw(g);
	door2.Draw(g);
	door3.Draw(g);
	movingwall.Draw(g);
	movingwalls.Draw(g);
	saws.Draw(g);
	keys.Draw(g);
	ammos.Draw(g);
	healths.Draw(g);
	staticmodels.Draw(g);
	animatedmodels.Draw(g);
	//ShowBoundingBoxes();
	//ShowCoordinateSystem();
}

void CMyGame::CameraControl(CGraphics* g)
{
	// game world tilt 
	float tilt = 70;
	float scale = 1.0f;

	// ------ Global Transformation Functions -----
	glTranslatef(0, -50, 400);

	glRotatef(tilt, 1, 0, 0);
	glScalef(scale, scale, scale);

	// ---- 3rd person camera setup -----
	glRotatef(90, 0, 1, 0);
	glTranslatef(-player.GetX(), 0, -player.GetZ());  

	UpdateView();
	Light.Apply();
}


// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
	ShowMouse();
	SDL_WM_GrabInput(SDL_GRAB_OFF);

	// title music
	levelmusic.Stop(); levelmusic.Clear();
	levelmusic.Play("titlemusic.wav", -1); levelmusic.SetVolume(30);

	// variables
    level=1;
	score = 0;

	//reset
	levelcompleted = false;
	collectedshotgun = false;
	helpscreenactive = false;

	// setup healthbar
	hbar.SetSize(130, 30);
	hbar.SetPosition(215, 55);
	hbar.SetHealth(100);
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
	HideMouse();
	if (level == 1) OnStartLevel(1);
	else if ( level == 2) OnStartLevel(2);
	else if (level == 3)  OnStartLevel(3);
}


// called when Game is Over
void CMyGame::OnGameOver()
{
	levelmusic.Stop();
}

// one time termination code
void CMyGame::OnTerminate()
{

}

// -------    Keyboard Event Handling ------------

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	
	if (sym == SDLK_SPACE)
	{
		if (IsPaused()) ResumeGame();
		else PauseGame();

	}
	if (sym == SDLK_F2) NewGame();
	
	// ------- Door Button Interaction -------------------
	if (sym == SDLK_e && player.HitTestFront(&door1) && hasbluekey && door1.GetStatus() == 0)
	{
		// remove wall which prevents imp walking through door
		for (CModel* wall : walls)
		{
			if (wall->GetStatus() == 1) wall->Delete();
		}
		walls.remove_if(deleted);

		door1.Open();
		dooropen.Play("dooropen.wav"); dooropen.SetVolume(20);
	}
	if (sym == SDLK_e && player.HitTestFront(&door2) && hasyellowkey && door2.GetStatus() == 0)
	{
		// remove wall which prevents imp walking through door
		for (CModel* wall : walls)
		{
			if (wall->GetStatus() == 2) wall->Delete();
		}
		walls.remove_if(deleted);

		door2.Open();
		dooropen.Play("dooropen.wav"); dooropen.SetVolume(20);
	}
	if (sym == SDLK_e && player.HitTestFront(&door3) && hasredkey && door3.GetStatus() == 0)
	{
		// remove wall which prevents imp walking through door
		for (CModel* wall : walls)
		{
			if (wall->GetStatus() == 3) wall->Delete();
		}
		walls.remove_if(deleted);

		door3.Open();
		dooropen.Play("dooropen.wav"); dooropen.SetVolume(20);
	}

	// weapon selection
	if (sym == SDLK_2)
	{
		usingshotgun = false;
		usingpistol = true;
	}
	if (sym == SDLK_3 && collectedshotgun)
	{
		usingpistol = false;
		usingshotgun = true;
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	
}

// -----  Mouse Events Handlers -------------

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	if (IsMenuMode())
	{
		// level 1 button
		if (level1button.HitTest(x, y) && level1button.GetStatus() != 1)
		{
			level1button.LoadImage("level1Press.bmp");
			level1button.SetStatus(1);
		}
		else if (!level1button.HitTest(x, y) && level1button.GetStatus() == 1)
		{
			level1button.LoadImage("level1Norm.bmp");
			level1button.SetStatus(0);
		}
		// level 2 button
		if (level2button.HitTest(x, y) && level2button.GetStatus() != 1)
		{
			level2button.LoadImage("level2Press.bmp");
			level2button.SetStatus(1);
		}
		else if (!level2button.HitTest(x, y) && level2button.GetStatus() == 1)
		{
			level2button.LoadImage("level2Norm.bmp");
			level2button.SetStatus(0);
		}
		// bonus level (3)
		if (bonusbutton.HitTest(x, y) && bonusbutton.GetStatus() != 1)
		{
			bonusbutton.LoadImage("bonusPress.bmp");
			bonusbutton.SetStatus(1);
		}
		else if (!bonusbutton.HitTest(x, y) && bonusbutton.GetStatus() == 1)
		{
			bonusbutton.LoadImage("bonusNorm.bmp");
			bonusbutton.SetStatus(0);
		}
		// help button
		if (helpbutton.HitTest(x, y) && helpbutton.GetStatus() != 1)
		{
			helpbutton.LoadImage("helpPress.bmp");
			helpbutton.SetStatus(1);
		}
		else if (!helpbutton.HitTest(x, y) && helpbutton.GetStatus() == 1)
		{
			helpbutton.LoadImage("helpNorm.bmp");
			helpbutton.SetStatus(0);
		}
	}

	if (IsRunning())
	{
		CVector pos = ScreenToFloorCoordinate(x, y);
		player.SetDirectionAndRotationToPoint(pos.x, pos.z);
		reticle.SetPosition(x, y + 15.0f);
	}
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{    
	if (IsMenuMode())
	{
		// difficulty
		if (easybutton.HitTest(x, y) && difficulty != 1.5)
		{
			difficulty = 1.5;
			easybutton.LoadImage("easyPress.bmp");
			normalbutton.LoadImage("normalNorm.bmp");
			hardbutton.LoadImage("hardNorm.bmp");
		}
		else if (normalbutton.HitTest(x, y) && difficulty != 1.0)
		{
			difficulty = 1.0;
			easybutton.LoadImage("easyNorm.bmp");
			normalbutton.LoadImage("normalPress.bmp");
			hardbutton.LoadImage("hardNorm.bmp");
		}
		else if (hardbutton.HitTest(x, y) && difficulty != 0.5)
		{
			difficulty = 0.5;
			easybutton.LoadImage("easyNorm.bmp");
			normalbutton.LoadImage("normalNorm.bmp");
			hardbutton.LoadImage("hardPress.bmp");
		}
		// levels
		if (level1button.HitTest(x, y))
		{
			level = 1;
			StartGame();
		}
		else if (level2button.HitTest(x, y))
		{
			level = 2;
			StartGame();
		}
		else if (bonusbutton.HitTest(x, y))
		{
			level = 3;
			StartGame();
		}
		// helpscreen
		if (!helpscreenactive)
		{
			if (helpbutton.HitTest(x, y)) helpscreenactive = true;
		}
		else 
		{
			if (helpscreen.HitTest(x, y)) helpscreenactive = false;
		}
	}
	if (IsGameOver())
	{
		if (levelcompleted)
		{
			if (rtnmenubtn.HitTest(x, y)) NewGame();
		}
	}

	// get coordinates
	//CVector pos = ScreenToFloorCoordinate(x, y);
	//cout << "x=" << pos.x << " " << "z=" << pos.z << endl;
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
