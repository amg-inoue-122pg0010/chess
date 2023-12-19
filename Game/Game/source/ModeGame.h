#pragma once
#include <string>
#include "ModeMenu.h"
#include <memory>
#include"ObjectBase.h"
#include"Player.h"
#include <vector> 
#include <unordered_map>

#include"Bishop.h"
#include"King.h"
#include"CharacterBase.h"

// 計算用マクロ
#define	PI	(3.1415926535897932386f)
#define	DEG2RAD(x)			( ((x) / 180.0f ) * PI )
#define	RAD2DEG(x)			( ((x) * 180.0f ) / PI )

// カメラ
class Camera {
public:
	VECTOR	_vPos;					// 位置
	VECTOR	_vTarget;				// カメラが追うターゲット座標
	float	_clipNear, _clipFar;	// クリップ
	VECTOR	_vPosDifference;        //ターゲットとカメラの距離
	float  _angleX;  //水平角度
	float  _angleY;  //水平角度

	float CamV = 0;
	float CamH = 0;

	//カメラ回転に使う極座標の計算
	void PolarCoords(float angle, float length, std::pair<float, float> centerPos,
		float &posX, float &posY) {
		//極座標で円での位置を求める
		float m_Length = length;	// 半径の長さ
		float m_CenterX = centerPos.first;	// 中心座標X
		float m_CenterY = centerPos.second;	// 中心座標Y
		// 中心座標に角度と長さを使用した円の位置を加算する
		// 度数法の角度を弧度法に変換
		float radius = Math::ToRadians(angle);

		// 三角関数を使用し、円の位置を割り出す。
		float add_x = cos(radius) * m_Length;
		float add_y = sin(radius) * m_Length;

		// 結果ででた位置を中心位置に加算した座標を返す
		posX = m_CenterX + add_x;
		posY = m_CenterY + add_y;
	}

	void CameraRotation();
};
// モード
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:

	std::shared_ptr<CharacterBase> _player;
	std::shared_ptr<CharacterBase> _enemy;

	bool _freeCamMode = false;

	ModeGame(int stn,int conte);
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

	bool GetDebugViewColloion() { return _bViewCollision; }
	void SetDebugViewColloion(bool b) { _bViewCollision = b; }
	bool GetDebugUseColloion() { return _bUseCollision; }
	void SetDebugUseColloion(bool b) { _bUseCollision = b; }
	bool GetDebugViewCameraInfo() { return _bViewCameraInfo; }
	void SetDebugViewCameraInfo(bool b) { _bViewCameraInfo = b; }

	bool GetDebugCup() { return _bViewCollision; }

	int GetStageFloorCollision() { return _stageCollisionFloor; };
	std::vector<int> GetStageWallCollision() { return _stageWallCollisions; }//デバッグ

	//レーザーの情報
	struct LaserData {
		std::vector<VECTOR> _vPoses;  //レーザーの始まりと終わりの座標
		int _type;  //レーザーの種類
		std::vector<int> _trapNums; //起動させるトラップの番号
		int _form;
		int _stageNum;
		bool _move;//動くか
		std::vector<VECTOR> _movedVPoses;
		bool _switch;//スイッチするか
		std::vector<VECTOR> _switchVPoses;
		std::vector<VECTOR> _switchMovedVPoses;
		//円形用
		float _delay;     //ステージスタートから起動するまでの時間
		float _spd;//拡大の速さ

	};
	std::vector<LaserData> _laserData;

	//トラップの情報
	struct TrapData {
		VECTOR _vPos;
		int _type;
		int _stageNum;
		VECTOR _movedVPos;
		std::vector<int> _switchLaserNum;
		int _spd;
	};
	std::vector<TrapData> _trapData;

	//プレイヤーの情報
	Player::PlayerData _playerData;

protected:

	// カメラ
	Camera	_cam;
	//Enemy  _enemy;
	//std::vector<base> objects;
	// 3Dモデル描画用
	int _handle;

	std::vector<std::shared_ptr<ObjectBase>> objects;

	bool _input;
	float _charaSpeed;
	int _score;

	//json読み込み
	//jsonファイルを読み込んで配列に情報をまとめる
	void LoadJsonfile(); 

	int _attach_index;
	float _total_time;
	float _play_time;
	VECTOR _vPos;	// 位置
	//int _g;//重力

	VECTOR _vDir;	// 向き
	
	// デバッグ用
	bool	_bViewCollision;
	bool	_bUseCollision;
	bool	_bViewCameraInfo;

	// マップ用
	int _handleMap;
	int _handleSkySphere;
	int _frameMapCollision;
	int _wallGraphHandle_front;//壁の絵
	int _wallGraphHandle_side;//壁の絵

	//床の当たり判定を無理やり実装
	std::vector<int> _stageWallCollisions;//デバッグ
	int _stColl;//デバッグ、ステージのコリジョン
	int _stColl2;//デバッグ、ステージのコリジョン
	int _stageCollisionFloor;

	//光源
	int _light;
	// シャドウマップ用
	int _handleShadowMap;
	int _handleShadowMap_2;

	//UIクラス
	std::vector<int> _hp;
	std::unordered_map<int, int> _displayTimes;
	std::unordered_map<int, int> _remain;

	int _screenBright = 0;//画面の明るさ

	int _conte;//コンティニュー回数
}; 


