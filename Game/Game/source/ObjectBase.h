#pragma once
#include "appframe.h"
#include "ApplicationMain.h"
#include "../../../AppFrame/Easing.h"

class ObjectBase {
public:
	ObjectBase() {};

	// ムーブコンストラクタ
	ObjectBase(ObjectBase&& obj) noexcept
		: _life(obj._life)
		, _vPos(obj._vPos)
	{
		//obj._p = NULL;ポインタ変数を空にするD:\２０２２井上\PG1年_CPP応用\221221_PG1年_CPP応用メモ12参照

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
	int _r;//円や球の当たり判定に使う半径
	VECTOR _vPos;
	float _spd;
	VECTOR _vDir;
	int _g;//重力
};

