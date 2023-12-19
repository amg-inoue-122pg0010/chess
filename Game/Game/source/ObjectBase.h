#pragma once
#include "appframe.h"
#include "ApplicationMain.h"
#include "../../../AppFrame/Easing.h"

class ObjectBase {
public:
	ObjectBase() {};

	// ���[�u�R���X�g���N�^
	ObjectBase(ObjectBase&& obj) noexcept
		: _life(obj._life)
		, _vPos(obj._vPos)
	{
		//obj._p = NULL;�|�C���^�ϐ�����ɂ���D:\�Q�O�Q�Q���\PG1�N_CPP���p\221221_PG1�N_CPP���p����12�Q��

	}
	ObjectBase& operator=(ObjectBase&& obj) noexcept;
	
	VECTOR GetvPos() { return _vPos; };
	void SetvPos(VECTOR pos) { _vPos = pos; }
	int Getrad() { return _r; };
	int Getlife() { return _life; };
	void Setlife(int life) {_life = life; };
	int Getspd() { return _spd; }
	void SetSpd(int spd) { _spd = spd; }
	VECTOR SetvDir(VECTOR dir) {
		_vDir = dir;
		return _vDir;
	}
	VECTOR GetvDir() { return _vDir; }

	int _handle;
	
protected:

	int _life;
	int _r;//�~�⋅�̓����蔻��Ɏg�����a
	VECTOR _vPos;
	float _spd;
	VECTOR _vDir;
	int _g;//�d��
};

