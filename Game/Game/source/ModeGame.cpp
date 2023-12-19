
#include "ApplicationMain.h"
#include "ModeGame.h"
#include <memory>
#include"ObjectBase.h"
#include "ModePause.h"


void Camera::CameraRotation() {

	//マウスでカメラ回転
	int x, y;
	GetMousePoint(&x, &y);
	if (x != 0) {

		const int sensitivity = 1;
		//極座標に使う角度の更新
		_angleX += x * -sensitivity;
		float length = VSize(VGet(_vPosDifference.x, 0, _vPosDifference.z));
		PolarCoords(_angleX, length, std::make_pair(_vTarget.x, _vTarget.z),
			_vPos.x, _vPos.z);
	}

	if (y != 0) {

		const int sensitivity = 1;
		float posy = 0;
		//極座標に使う角度の更新
		_angleY += y * -sensitivity;
		float length = VSize(VGet(_vPosDifference.x, 0, _vPosDifference.y));
		PolarCoords(_angleY, length, std::make_pair(_vTarget.x, _vTarget.y),
			posy, _vPos.y);
	}
	/*
	if (CamV > MaxV)	CamV = MaxV;
	if (CamV < MinV)	CamV = MinV;
	if (CamH > MaxH)	CamH -= MaxH;
	if (CamH < MinH)	CamH += MaxH;
	if (CamR > MaxR)	CamR = MaxR;
	if (CamR < MinR)	CamR = MinR;
	*/
	const int sensitivity = 1;
	CamV += y * sensitivity;
	CamH += x * sensitivity;
	MATRIX RotV = MGetRotX(CamV);
	MATRIX RotH = MGetRotY(CamH);
	_vPos = VTransform(VTransform(VGet(0.0f, 0.0f, -437), RotV), RotH);

	SetMousePoint(0, 0);

}


// メニュー項目
class MenuItemViewCollision : public MenuItemBase {
public:
	MenuItemViewCollision(void* param, std::string text) : MenuItemBase(param, text) {}

	// 項目を決定したらこの関数が呼ばれる
	// return int : 0 = メニュー継続, 1 = メニュー終了
	virtual int Selected() {
		ModeGame *mdGame = static_cast<ModeGame*>(_param);
		mdGame->SetDebugViewColloion(!mdGame->GetDebugViewColloion());
		return 1;
	}
};

class MenuItemUseCollision : public MenuItemBase {
public:
	MenuItemUseCollision(void* param, std::string text) : MenuItemBase(param, text) {}

	// 項目を決定したらこの関数が呼ばれる
	// return int : 0 = メニュー継続, 1 = メニュー終了
	virtual int Selected() {
		ModeGame* mdGame = static_cast<ModeGame*>(_param);
		mdGame->SetDebugUseColloion(!mdGame->GetDebugUseColloion());
		return 1;
	}
};

class MenuItemViewCameraInfo : public MenuItemBase {
public:
	MenuItemViewCameraInfo(void* param, std::string text) : MenuItemBase(param, text) {}

	// 項目を決定したらこの関数が呼ばれる
	// return int : 0 = メニュー継続, 1 = メニュー終了
	virtual int Selected() {
		ModeGame* mdGame = static_cast<ModeGame*>(_param);
		mdGame->SetDebugViewCameraInfo(!mdGame->GetDebugViewCameraInfo());
		return 1;
	}
};

ModeGame::ModeGame(int stn,int conte)
{
	_conte = conte;
	//jsonファイル読み込み
	LoadJsonfile();

	//プレイヤーの登録
	auto p = std::make_shared<King>();
	//auto p = std::make_shared<Bishop>();
	_player = p;
	_player->SetCharacterType(CharacterBase::CharacterType::PLAYER);
	_player->CharacterObbSet();

	//敵の登録
	auto e = std::make_shared<Bishop>();
	_enemy = e;
	_enemy->SetvPos(VGet(500,0,500));
	_enemy->SetCharacterType(CharacterBase::CharacterType::ENEMY);
	_enemy->CharacterObbSet();
	
}

bool ModeGame::Initialize() {
	if (!base::Initialize()) { return false; }

	//ステージに使う2D画像
	_wallGraphHandle_front = ResourceServer::LoadGraph("res/stage/wall/front.png");
	_wallGraphHandle_side = ResourceServer::LoadGraph("res/stage/wall/side.png");

	_attach_index = -1;		// アニメーションアタッチはされていない
	// ステータスを「無し」に設定
	//_status = STATUS::NONE;
	// 再生時間の初期化
	_total_time = 0.f;
	_play_time = 0.0f;
	// 位置,向きの初期化
	_vPos = VGet(0, 0, 0);
	_vDir = VGet(0, 0, -1);		// キャラモデルはデフォルトで-Z方向を向いている

	_input = true;
	_charaSpeed = 6.f;

	// シャドウマップの生成
	_handleShadowMap = MakeShadowMap(4096, 4096);
	_handleShadowMap_2 = MakeShadowMap(4096, 4096);

#if 1
	auto chessboard = ResourceServer::GetHandles("ChessBoard");
	MV1SetPosition(chessboard, VGet(4650, -30, -3150));
	MV1SetScale(chessboard, VGet(0.3f, 0.3f, 0.3f));

	_stageCollisionFloor = MV1LoadModel("res/TT_COR_0326.mv1");
	MV1SetPosition(_stageCollisionFloor, VGet(0, -30, 0));
	MV1SetScale(_stageCollisionFloor, VGet(1200, 2, 300));

	//右壁
	auto stageCollisionWallL = MV1DuplicateModel(_stageCollisionFloor);
	MV1SetPosition(stageCollisionWallL, VGet(5266, 200, 4500));
	MV1SetScale(stageCollisionWallL, VGet(2000, 0, 1000));
	MV1SetRotationXYZ(stageCollisionWallL, VGet(0, 0, Math::ToRadians(90)));
	//左壁
	auto stageCollisionWallR = MV1DuplicateModel(_stageCollisionFloor);
	MV1SetPosition(stageCollisionWallR, VGet(-4273, 200, 4500));
	MV1SetScale(stageCollisionWallR, MV1GetScale(stageCollisionWallL));
	MV1SetRotationXYZ(stageCollisionWallR, VGet(0, 0, Math::ToRadians(90)));
	//手前壁
	auto stageCollisionFront = MV1DuplicateModel(_stageCollisionFloor);
	MV1SetPosition(stageCollisionFront, VGet(0, 0, -3762));
	MV1SetScale(stageCollisionFront, VGet(2000, 3, 150));
	MV1SetRotationXYZ(stageCollisionFront, VGet(Math::ToRadians(90), 0, Math::ToRadians(90)));
	//奥壁
	auto stageCollisionBack = MV1DuplicateModel(_stageCollisionFloor);
	MV1SetPosition(stageCollisionBack, VGet(0, 0, 5762));
	MV1SetScale(stageCollisionBack, MV1GetScale(stageCollisionFront));
	MV1SetRotationXYZ(stageCollisionBack, VGet(Math::ToRadians(90), 0, Math::ToRadians(90)));

	_stageWallCollisions.push_back(stageCollisionWallR);
	_stageWallCollisions.push_back(stageCollisionWallL);
	_stageWallCollisions.push_back(stageCollisionFront);
	_stageWallCollisions.push_back(stageCollisionBack);
	_stageWallCollisions.push_back(chessboard);

	// ステージ
	_handleMap = MV1LoadModel("res/stage/stagecube_0315(tex,mv1)/Stagecube1_0315.mv1");
	MV1SetRotationXYZ(_handleMap, VGet(0, Math::ToRadians(90), 0));
	MV1SetPosition(_handleMap, VGet(0, 0, 0));

	MV1SetMeshBackCulling(_handleMap,0,  true);
	
	//光源
	//_light = CreateDirLightHandle(VGet(-1, 1, -1));
#else
	// フィールド
	_handleMap = MV1LoadModel("res/Ground/Ground.mv1");
	_frameMapCollision = MV1SearchFrame(_handleMap, "ground_navmesh");
#endif
	// コリジョン情報の生成
	//MV1SetupCollInfo(_handleMap, _frameMapCollision, 16, 16, 16);

	MV1SetupCollInfo(_stageCollisionFloor, _frameMapCollision, 16, 16, 16);
	// コリジョンのフレームを描画しない設定
	MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);

#if 1
	_cam._vPos = VGet(175.f, 150.f, -270.f);
	_cam._vTarget = VGet(0, 0, 0);
	_cam._vPosDifference = VGet(0.f, 200.f, 350.f);
	_cam._clipNear = 2.f;
	_cam._clipFar = 10000.f;
#else

#endif
	// その他初期化
	_bViewCollision = TRUE;
	_bUseCollision = TRUE;
	_bViewCameraInfo = false;

	return true;
}

bool ModeGame::Terminate() {
	base::Terminate();
	DeleteLightHandle(_light);
	return true;
}



bool ModeGame::Process() {
	base::Process();		
	++gGlobal._time;

	int key = ApplicationMain::GetInstance()->GetKey();
	int trg = ApplicationMain::GetInstance()->GetTrg();
	int rel = ApplicationMain::GetInstance()->GetRel();

	if(trg & PAD_INPUT_10) {
		_freeCamMode = !_freeCamMode;
		SetMousePoint(0, 0);
	}

	if(_freeCamMode) {
		
		//_cam._vPosDifference = VSub( _cam._vPos,_cam._vTarget);
	}
	else {

		//カメラの操作
		_cam.CameraRotation();
		// カメラの向いている角度を取得
		float sx = _cam._vPos.x - _cam._vTarget.x;
		float sz = _cam._vPos.z - _cam._vTarget.z;
		float camrad = -atan2(sx, sz);

		VECTOR oldPlayerVPos = _player->GetvPos();
		//プレイヤーのprocessを呼ぶ
		_player->PlayerProcess(camrad, _enemy->GetvPos());
		_enemy->EnemyProcess(_player->GetvPos());

		//敵とプレイヤーのダメージ判定
		OBB pc;
		std::vector<OBB> pd;
		for (auto po : _player->GetObbs()) {
			if (po.GetObbType() == OBB::Type::CHARACTER) {
				pc = po;
			}
			if (po.GetObbType() == OBB::Type::DAMAGE ||
				po.GetObbType() == OBB::Type::BULLET) {
				pd.push_back(po);
			}
		}

		OBB ec;
		for (auto eo : _enemy->GetObbs()) {
			OBB ed;
			if (eo.GetObbType() == OBB::Type::DAMAGE||
				eo.GetObbType() == OBB::Type::BULLET) {
				ed = eo;
				if (_player->GetObb().ColOBBs(pc, ed)) {
					_player->Damage();
				}
			}
			
			if (eo.GetObbType() == OBB::Type::CHARACTER) {
				ec = eo;
			}
		}

		for (auto d : pd) {
			if (_player->GetObb().ColOBBs(ec, d)) {
				_enemy->Damage();
			}
		}

		//_player->GetObb().ColOBBs();

		//カメラの座標の更新
		_cam._vPos = VAdd(_cam._vPos, VSub(_player->GetvPos(), oldPlayerVPos));
		_cam._vTarget = VAdd(_player->GetvPos(), VGet(0.f, 100.f, 0.f));

		if(trg & PAD_INPUT_9) {
			// 上キー+ESCキーでデバッグメニュー画面を開く
			ModeMenu* modeMenu = new ModeMenu();
			// ModeGameより上のレイヤーにメニューを登録する
			ModeServer::GetInstance()->Add(modeMenu, 99, "menu");
			// ModeMenuにメニュー項目を追加する
			modeMenu->Add(new MenuItemViewCollision(this, "ViewCollision"));
			modeMenu->Add(new MenuItemUseCollision(this, "UseCollision"));
			modeMenu->Add(new MenuItemViewCameraInfo(this, "ViewCameraInfo"));
		}
		/*else if (trg & PAD_INPUT_8 || trg & PAD_INPUT_12) {
		// ESCキーでポーズ画面を開く
		ModePause* modePause = new ModePause(this);
		ModeServer::GetInstance()->Add(modePause, 99, "pause");
		PlaySoundMem(gGlobal._se["Pause"], DX_PLAYTYPE_BACK, true);
	}*/

	// デバッグ機能
		if(_bViewCollision) {
			MV1SetFrameVisible(_handleMap, _frameMapCollision, TRUE);
		}
		else {
			MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);
		}

	}
	

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();
	return true;
}

bool ModeGame::Render() {

	base::Render();
	auto color = GetColor(rand() % 255, rand() % 255, rand() % 255);

	// 3D基本設定
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseBackCulling(TRUE);

	// ライト設定
	SetUseLighting(true);
#if 1	// 平行ライト
	SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
	ChangeLightTypeDir(VGet(0, -1, 0));
#endif
#if 0	// ポイントライト
	SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
	ChangeLightTypePoint(VAdd(_vPos,VGet(0,50.f,0)), 1000.f, 0.f, 0.005f, 0.f);
#endif

	// カメラ設定更新
	SetCameraPositionAndTarget_UpVecY(_cam._vPos, _cam._vTarget);
	SetCameraNearFar(_cam._clipNear, _cam._clipFar);
	
	// シャドウマップが想定するライトの方向もセット
	SetShadowMapLightDirection(_handleShadowMap, VGet(-1, -0.1f, 0));
	SetShadowMapLightDirection(_handleShadowMap_2, VGet(0, -1, 0));
	// シャドウマップに描画する範囲を設定
	//SetShadowMapDrawArea(_handleShadowMap, VGet(-600.0f, -1.0f, -500.0f), VGet(600.0f, 500.0f, 9200.0f));//VGet(0.0f, -1.0f, 0.0f), VGet(600.0f, 1200.0f, 9200.0f));

	SetShadowMapDrawArea(_handleShadowMap, VGet(-800.f, _player->GetvPos().y - 200.f, _player->GetvPos().z - 40.f),
		VGet(_player->GetvPos().x + 200.f, _player->GetvPos().y + 200.f, _player->GetvPos().z + 40.0f));
	SetShadowMapDrawArea(_handleShadowMap_2, VGet(-800.f, _player->GetvPos().y - 200.f, _player->GetvPos().z - 40.f),
		VGet(_player->GetvPos().x + 200.f, _player->GetvPos().y + 200.f, _player->GetvPos().z + 40.0f));
	// 2回まわして、path=0:シャドウマップへの描画, path=1:モデルの描画
	for (int path = 0; path < 4; path++) {
		if (path == 0) {
			// シャドウマップへの描画の準備
			ShadowMap_DrawSetup(_handleShadowMap);

		}
		else if (path == 1) {
			// シャドウマップへの描画を終了
			ShadowMap_DrawEnd();
			// 描画に使用するシャドウマップを設定
			SetUseShadowMap(0, _handleShadowMap);
		}
		if (path == 2) {
			ShadowMap_DrawSetup(_handleShadowMap_2);
		}

		else if (path == 3) {
			// シャドウマップへの描画を終了
			ShadowMap_DrawEnd();
			// 描画に使用するシャドウマップを設定
			SetUseShadowMap(1, _handleShadowMap_2);
		}
		// プレイヤー描画
		
		if (path == 1 || path == 3) {
			SetUseShadowMap(0, -1);
			SetUseShadowMap(1, -1);
			_player->Render();
			_enemy->Render();
			SetUseShadowMap(0, _handleShadowMap);
			SetUseShadowMap(1, _handleShadowMap_2);
		}
		else {
		_player->Render();
		_enemy->Render();
		}
		
		
		if (path == 1 || path == 3) {
			// マップモデルを描画する
				//MV1SetPosition(_handleMap,VGet(100, 100, 100));
			//MV1DrawModel(_handleMap);
			if (_bViewCollision) {
				for (auto e : _stageWallCollisions) {
					MV1DrawModel(e);
				}
				MV1DrawModel(_stageCollisionFloor);
			}
		}

		// モデルを描画する
		if (path == 1 || path == 3) {
			VECTOR vRot = { 0,0,0 };
		}
	}
	// 描画に使用するシャドウマップの設定を解除
	SetUseShadowMap(0, -1);
	SetUseShadowMap(1, -1);

	// カメラ情報表示
	if (_bViewCameraInfo) {
		int x = 0, y = 0, size = 16;
		SetFontSize(size);
		DrawFormatString(x, y, GetColor(255, 0, 0), "Camera:"); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  target = (%5.2f, %5.2f, %5.2f)", _cam._vTarget.x, _cam._vTarget.y, _cam._vTarget.z); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _cam._vPos.x, _cam._vPos.y, _cam._vPos.z); y += size;
		DrawFormatString(x, y + 150, GetColor(255, 0, 0), "SCORE = TIME = %d", gGlobal._time);
		float sx = _cam._vPos.x - _cam._vTarget.x;
		float sz = _cam._vPos.z - _cam._vTarget.z;
		float length = sqrt(sz * sz + sx * sx);
		float rad = atan2(sz, sx);
		float deg = RAD2DEG(rad);
		DrawFormatString(x, y, GetColor(255, 0, 0), "  len = %5.2f, rad = %5.2f, deg = %5.2f", length, rad, deg); y += size;

		DrawFormatString(0, 350, GetColor(255, 0, 0), "レーザーコリジョン = %d", (int)_bUseCollision);
	}
	
	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();
	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();
	
	return true;
}

//json
#include <stdio.h>
#include <stdlib.h>
#include "CFile/CFile.h"
#include "CFile/utf8sjis.h"
#include "CFile/picojson.h"

void ModeGame::LoadJsonfile()
{


	// データの確認-----------------------------------------------------------

	//プレイヤー情報の読み込み
	CFile player("Player.json");
	if (!player.data()) { printf("read error."); return; }

	std::string s = (char*)player.data();

	// json
	picojson::value playerjson;
	picojson::parse(playerjson, s);
	picojson::object jsRoot = playerjson.get<picojson::object>();

	picojson::array sheet = jsRoot["Player"].get<picojson::array>();

		int jumpHeight;
		int airJumpHeight;
		float walkSpd;
		float dashSpd;
		float maxDashSpd;
		float _accelerateTime;
		float squatSpd;
		float dashTimeToSlidhing;

		picojson::object p = sheet[0].get<picojson::object>();
		jumpHeight      = (int)p["jumpHeight"].get<double>();
		airJumpHeight   = (int)p["airJumpHeight"].get<double>();
		walkSpd       = (float)p["walkSpd"].get<double>();
		dashSpd       = (float)p["dashSpd"].get<double>();
		maxDashSpd       = (float)p["maxDashSpd"].get<double>();
		_accelerateTime = (float)p["accelerateTime"].get<double>();
		squatSpd      = (float)p["squatSpd"].get<double>();
		dashTimeToSlidhing = (float)p["dashTimeToSlidhing"].get<double>();

		Player::PlayerData Playerdata{ jumpHeight,airJumpHeight 
			,walkSpd,dashSpd,maxDashSpd,_accelerateTime,
			squatSpd,dashTimeToSlidhing };

		_playerData = Playerdata;

	/*
	// number
	printf("age: %d\n", (int)jsRoot["age"].get<double>());


	// bool
	printf("dead: %d\n", jsRoot["dead"].get<bool>());

	// string
	std::string strName = jsRoot["name"].get<std::string>();		// ただしUTF-8で入る
	printf("name(UTF8): %s\n", strName.c_str());		// コンソールウィンドウは標準ではSJIS表示なので文字化けする

	// string : UTF-8 -> SJIS
	printf("name(SJIS): %s\n", ConvUTF8toSJIS(strName).c_str());

	// array
	picojson::array a = jsRoot["birth"].get<picojson::array>();
	printf("birth: %d / %d / %d\n", (int)a[0].get<double>(), (int)a[1].get<double>(), (int)a[2].get<double>());
	*/
}