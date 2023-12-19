#pragma once
#include "ObjectBase.h"
#include "ApplicationGlobal.h"
class CharacterBase :
    public ObjectBase
{
public:
	CharacterBase();
	virtual void Init();
	virtual void CharacterObbSet();
	void BodyRegionVPosUpdate();//体のobbの上面と底面の座標を更新
	virtual void Process();
	virtual void PlayerProcess(float camVDir, VECTOR target);
	virtual void EnemyProcess(VECTOR target);
	virtual void Move();
	virtual void EnemyMove();
	virtual void Roll();
	virtual void RollFront();
	virtual void Attack();
	virtual void ClassAttackLow();
	virtual void ClassAttackHigh();
	virtual void Damage();
	virtual void AnimationSet(bool interruption);//引数には再生するアニメーションの中断の可否を渡す
	virtual void AnimationPlay();//セットされたアニメーションを再生
	virtual bool AnimationNoInterruption();//中断できないアニメーションが再生中か
	virtual void SetAnimeNoInterruption(bool interruption);//再生中のアニメーションの中断の可否を設定する
	virtual void Render();

	std::vector<OBB> GetObbs() { return _obbs; }
	OBB GetObb() { return _obb; }

	enum class STATUS {
		NONE,
		IDLE,
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
		DEAD,
		ROLL,
		ROLLFRONT,
		ATTACK,
		CLASSATTACKLOW,
		CLASSATTACKHIGH,
		_EOT_
	};
	STATUS _status;
	STATUS _oldStatus;

	enum class CharacterType {
		PLAYER,
		ENEMY,
		NONE,
	};
	CharacterType _characterType;
	void SetCharacterType(CharacterType charatype) {
		_characterType = charatype;
	};

protected:
	CharacterBody _charaBody;
	float _CharacterCenterHight;//キャラの中心の位置の高さ
	//キャラの動ける範囲
	VECTOR _maxVPos;
	VECTOR _minVPos;
    OBB _obb;
	std::vector<OBB> _obbs;
	//モーションハンドル
	int _motionHandleIdle;
	int _motionHandleWalk;
	int _motionHandleDash;
	int _motionHandleRoll;
	int _motionHandleRollFront;
	int _motionHandleDamage;
	int _motionHandleDead;
	int _motionHandleAttack;
	int _motionHandleClassAtkLow;//固有攻撃•弱
	int _motionHandleClassAtkHigh;//固有攻撃•強
	int _oldmotionHandle;
	bool _land; //着地しているかどうか
	//被ダメージで怯んでる時間(無敵)
	int _nohittime;  
	int _playerNohittime;  
	int _EnemyNohittime;  
	bool _hit;  //被ダメ中か
	VECTOR _normalVDir;
	float _dashSpd;
	float _rad;
	int _enemyActionInterval;//敵の行動の間隔
	bool _action;//アクション中かどうか
	VECTOR _target;//敵側で召喚された時、追いかける対象の座標
	float _cameraVDir;
	float _bulletSpd;  //遠距離攻撃の動く速さ
	float _range;//キャラが敵扱いの時、プレイヤーとの間合い管理に使う
	//攻撃の発生隙
	int _attackStartupFrame;
	int _classAtkLowStartupFrame;
	int _classAtkHighStartupFrame;
	int _recoveryFrame;//攻撃の後隙

	std::function<VECTOR(VECTOR, VECTOR, float)> _movementInAction;   //アクション中の動き
	float _moveInActSpd;//アクション中に動く速さ
	float _motionSpd;//モーションの再生速度


	// アニメーション複数管理
	class Animation {
	public:
		Animation() {}
		virtual ~Animation() {}
		int _attach_index;
		float _total_time;
		float _play_time;
		// モーションブレンド用
		float _close_total_time;
		float _close_time;
		float _anim_spd;
		bool _noInterruption;//アニメーションを途中で中断させないかどうか
		bool _nohitAnime;
	};
	std::vector<Animation*> _vAnim;
};

