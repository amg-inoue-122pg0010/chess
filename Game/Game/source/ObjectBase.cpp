#include "ObjectBase.h"

ObjectBase& ObjectBase::operator=(ObjectBase&& obj) noexcept {
	// �������玩���ւ̃��[�u�͂��Ȃ�
	/*
	if (this != &obj) {
		if (_p) { delete[] _p; }	// �����̃��������폜
		_p = obj._p;				// ����̃������������p��
		obj._p = NULL;				// ����̃������Q�Ƃ�NULL��
	}
	*/
	_life = obj._life;
	//�����ő���Ǝ����̕ϐ������ւ���
	return *this;	// ������Ԃ�
}