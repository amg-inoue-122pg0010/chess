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

	//�v���C���[�̏��
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

	VECTOR _v;//�v���C���[�̈ړ��ʂ�ۑ�����
	float length;
	float rad;
	bool _camerarequest;//�J�����𓮂������ǂ���
	VECTOR GetMovement() { return _v; }
	int Setnohittime(int n) { _nohittime = n; return _nohittime; } //��_����̖��G����
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

	// �A�j���[�V���������Ǘ�
	class Animation {
	public:
		Animation() { 
			
			_anim_spd = 0.5f;
			_anim_step = 0;//�f�o�b�O�p���Ⴊ�݂Ɏg������
		}
		virtual ~Animation() {}
		int _attach_index;
		float _total_time;
		float _play_time;
		// ���[�V�����u�����h�p
		float _close_total_time;
		float _close_time;
		float _anim_spd;
		int _anim_step;//�A�j���̐i�s�i�K
	};
	std::vector<Animation*> _vAnim;
	bool animReset = false;

	VECTOR _senterpos = VGet(0, 0, 0);
	VECTOR _capsulepos_end = VGet(0, 0, 0);

protected:
	int _nohittime;  //��_���[�W�ŋ���ł鎞��(���G)
	bool _hit;
	bool _stop;
	int _height;//�v���C���[�̍���
	int _standardHeight;//�����Ă鎞�̐g��
	int _squatHeight;//���Ⴊ�񂾎��̐g��
	bool _land; //���n���Ă��邩�ǂ���
	int _airJumpCnt;
	int _wallJumpCnt;
	bool _stick; //�ǂɓ������Ă��邩�ǂ���
	float _colSubY;	// �R���W�������莞��Y�␳(���ʒu�j
	float _colSubSide;  //�ǃR���W�������莞�̐����␳
	float _wallJumpDir; //�ǃW�����v�������
	//�G�t�F�N�g
	int _playingEffectDamage;
	int _playingEffect;
	int _damage;
	int _sliding;
	int _doublejump;
	int _die;


	ModeGame& _modegame;
	int _standbyInterval;//�ҋ@���[�V�����̊Ԋu
	int _standbytype;//�ҋ@���[�V�����̎��

	float _dashTime;//���������ɑ���������
	VECTOR _slidhingV;//�X���C�f�B���O���Ă�����A�v�Z�O��ۑ�
	int _jumpHeight;
	int _airJumpHeight;
	float _walkSpd;
	float _dashSpd;
	float _minDashSpd;//�_�b�V���̍Œᑬ�x
	float _maxDashSpd;//�_�b�V���̍ő呬�x
	float _accelerateTime;//�_�b�V�����x���ő�ɂȂ�܂ł̎���
	float _acceleration;//�����x
	float _squatSpd;
	float _dashTimeToSlidhing;//�X���C�f�B���O�ɕK�v�ȃ_�b�V������

private:

};

