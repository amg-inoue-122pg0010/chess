#include "ObjectBase.h"

ObjectBase& ObjectBase::operator=(ObjectBase&& obj) noexcept {
	// 自分から自分へのムーブはしない
	/*
	if (this != &obj) {
		if (_p) { delete[] _p; }	// 自分のメモリを削除
		_p = obj._p;				// 相手のメモリを引き継ぐ
		obj._p = NULL;				// 相手のメモリ参照をNULLに
	}
	*/
	_life = obj._life;
	//ここで相手と自分の変数を入れ替える
	return *this;	// 自分を返す
}