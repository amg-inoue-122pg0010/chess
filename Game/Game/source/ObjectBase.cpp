#include "ObjectBase.h"

ObjectBase& ObjectBase::operator=(ObjectBase&& obj) noexcept {
	// ©•ª‚©‚ç©•ª‚Ö‚Ìƒ€[ƒu‚Í‚µ‚È‚¢
	/*
	if (this != &obj) {
		if (_p) { delete[] _p; }	// ©•ª‚Ìƒƒ‚ƒŠ‚ğíœ
		_p = obj._p;				// ‘Šè‚Ìƒƒ‚ƒŠ‚ğˆø‚«Œp‚®
		obj._p = NULL;				// ‘Šè‚Ìƒƒ‚ƒŠQÆ‚ğNULL‚É
	}
	*/
	_life = obj._life;
	//‚±‚±‚Å‘Šè‚Æ©•ª‚Ì•Ï”‚ğ“ü‚ê‘Ö‚¦‚é
	return *this;	// ©•ª‚ğ•Ô‚·
}