
#include "ApplicationMain.h"
#include "ApplicationGlobal.h"
#include "ModeGame.h"

// 実体
ApplicationMain				g_oApplicationMain;

const ResourceServer::MV1ModelMap models{
	{"CollBoard", { "res/TT_COR.mv1" }},
	{ "FallRock", {"res/Gimmick/rakuseki(texture,mv1)/rakuseki0221.mv1"} },
	{ "ApproachingWall", {"res/Gimmick/ApproachingWall/Trap.mv1"} },
	{ "MovingFloor", {"res/Gimmick/idouyuka(texture,mv1)/idouyuka0208_1.mv1"} },
	{ "GravityFloor", {"res/Gimmick/juuryoku(tex,mv1)/juuryoku_yuka1.mv1"} },
	{ "Switch", {"res/Gimmick/switch(tex,mv1)/switch_yuka_off1.mv1"} },
	{ "SwitchOn", {"res/Gimmick/switch(tex,mv1)/switch_yuka_on1.mv1"} },
	{ "Hammer", {"res/Gimmick/hammer_0315(tex,mv1)/hammer0315_j2.mv1"} },
	{ "OutputDeviceStraight", {"res/OutputDevice/straight/straight.mv1"} },
	{ "OutputDeviceCircle", {"res/OutputDevice/circle/beam_output_circle.mv1"} },

	{ "ChessBoard", {"res/stage/floor/Chess/board.mv1"} },

	{ "Damage", {"res/Character/usemotion/Damege.mv1"} },
	{ "Dead", {"res/Character/usemotion/Dead.mv1"} },
	{ "Idle", {"res/Character/usemotion/Idle.mv1"} },
	{ "Punch", {"res/Character/usemotion/Punch.mv1"} },
	{ "Roll", {"res/Character/usemotion/Roll.mv1"} },
	{ "RollFront", {"res/Character/usemotion/RollFront.mv1"} },
	{ "Run", {"res/Character/usemotion/Run.mv1"} },
	{ "Walk", {"res/Character/usemotion/Walk.mv1"} },

	//ビショップ
	{ "Bishop", {"res/Character/bishop/Bishop.mv1"} },
	{ "BishopAtkLow", {"res/Character/bishop/BishopAtkLow.mv1"} },
	{ "BishopAtkHigh", {"res/Character/bishop/BishopAtkHigh.mv1"} },

	//キング
	{ "King", {"res/Character/king/King.mv1"} },
	{ "KingNormalAtk", {"res/Character/king/KingNormalAtk.mv1"} },
	{ "KingAtkLow",   {"res/Character/king/KingAtkLow.mv1"} },
	{ "KingAtkHigh",   {"res/Character/king/KingAtkHigh.mv1"} },
	{ "KingIdle",      {"res/Character/king/kingIdle.mv1"} },
	{ "KingWalk",      {"res/Character/king/kingWalk.mv1"} },
	{ "KingRun",       {"res/Character/king/kingRun.mv1"} },
	{ "KingRoll",      {"res/Character/king/kingRoll.mv1"} },
	{ "KingRollFront", {"res/Character/king/kingRollFront.mv1"} },
	{ "KingDamage",    {"res/Character/king/kingDamage.mv1"} },
	{ "KingDead",      {"res/Character/king/kingDead.mv1"} },
											
};

bool ApplicationMain::Initialize(HINSTANCE hInstance) {
	if (!base::Initialize(hInstance)) { return false; }

	// アプリケーショングローバルの初期化
	gGlobal.Init();

	// モードの登録

	SetUseASyncLoadFlag(true);
	ResourceServer::LoadModels(models);
	SetUseASyncLoadFlag(false);
	ModeServer::GetInstance()->Add(new ModeGame(1,3), 1, "game");

	return true;
}

bool ApplicationMain::Terminate() {
	base::Terminate();
	return true;
}


bool ApplicationMain::Input() {
	base::Input();
	return true;
}

bool ApplicationMain::Process() {
	base::Process();
	return true;
}

bool ApplicationMain::Render() {
	base::Render();
	return true;
}

