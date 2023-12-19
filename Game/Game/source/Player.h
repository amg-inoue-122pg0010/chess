#pragma once
#include "ObjectBase.h"
class ModeGame;

class Player : public ObjectBase {
public:

	OBB _obb;

	Player(ModeGame& modegame);

	void Init();

	void Process();

	void Render();

	void Damage(int knockbacktime, int damage);

	void StopEffect();

	//プレイヤーの情報
	struct PlayerData {
		int _jumpHeight;
		int _airJumpHeight;
		float _walkSpd;
		float _dashSpd;
		float _maxDashSpd;
		float _accelerateTime;
		float _squatSpd;
		float _dashTimeToSlidhing;
	};

	void SetStatus(PlayerData _data);

	VECTOR _v;//プレイヤーの移動量を保存する
	float length;
	float rad;
	bool _camerarequest;//カメラを動かすかどうか
	VECTOR GetMovement() { return _v; }
	int Setnohittime(int n) { _nohittime = n; return _nohittime; } //被ダメ後の無敵時間
	int Getnohittime() { return _nohittime; }
	bool Setstop(bool n) { _stop = n; return _stop; }
	bool Getstop() { return _stop; }
	int Getheight() { return _height; };
	bool Getland() { return _land; };
	bool GetStick() { return _stick; }
	float GetDashSpd() { return _dashSpd; }
	void SetDashSpd(float spd) { _dashSpd = spd; }
	void AnimationSet();
	bool HitCheckWall(VECTOR a, VECTOR b, VECTOR oldvPos, VECTOR oldv, int e, bool stick);
	bool HitCheckFloor(MV1_COLL_RESULT_POLY hitPoly, VECTOR oldvPos);
	enum class STATUS {
		NONE,
		WAIT,
		WALK,
		JUMP,
		JUMP_AIR,
		JUMP_WALL,
		FALL,
		LANDING,
		SQUAT,
		SQUAT_WALK,
		DASH,
		SLIDHING,
		DAMAGE,
		DAMAGE_AIR,
		_EOT_
	};
	STATUS _status;
	STATUS _oldstatus;

	// アニメーション複数管理
	class Animation {
	public:
		Animation() { 
			
			_anim_spd = 0.5f;
			_anim_step = 0;//デバッグ用しゃがみに使うかも
		}
		virtual ~Animation() {}
		int _attach_index;
		float _total_time;
		float _play_time;
		// モーションブレンド用
		float _close_total_time;
		float _close_time;
		float _anim_spd;
		int _anim_step;//アニメの進行段階
	};
	std::vector<Animation*> _vAnim;
	bool animReset = false;

	VECTOR _senterpos = VGet(0, 0, 0);
	VECTOR _capsulepos_end = VGet(0, 0, 0);

protected:
	int _nohittime;  //被ダメージで怯んでる時間(無敵)
	bool _hit;
	bool _stop;
	int _height;//プレイヤーの高さ
	int _standardHeight;//立ってる時の身長
	int _squatHeight;//しゃがんだ時の身長
	bool _land; //着地しているかどうか
	int _airJumpCnt;
	int _wallJumpCnt;
	bool _stick; //壁に当たっているかどうか
	float _colSubY;	// コリジョン判定時のY補正(腰位置）
	float _colSubSide;  //壁コリジョン判定時の線分補正
	float _wallJumpDir; //壁ジャンプする方向
	//エフェクト
	int _playingEffectDamage;
	int _playingEffect;
	int _damage;
	int _sliding;
	int _doublejump;
	int _die;


	ModeGame& _modegame;
	int _standbyInterval;//待機モーションの間隔
	int _standbytype;//待機モーションの種類

	float _dashTime;//同じ方向に走った時間
	VECTOR _slidhingV;//スライディングしてる方向、計算前を保存
	int _jumpHeight;
	int _airJumpHeight;
	float _walkSpd;
	float _dashSpd;
	float _minDashSpd;//ダッシュの最低速度
	float _maxDashSpd;//ダッシュの最大速度
	float _accelerateTime;//ダッシュ速度が最大になるまでの時間
	float _acceleration;//加速度
	float _squatSpd;
	float _dashTimeToSlidhing;//スライディングに必要なダッシュ時間

private:

};

