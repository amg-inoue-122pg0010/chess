#pragma once

#include "appframe.h"

class ApplicationGlobal
{
public:
	ApplicationGlobal();
	virtual ~ApplicationGlobal();

	bool Init();

public:
	int		_cgCursor;
	int _time = 0;
}; 

// ���̃\�[�X��gGlobal���g����悤��
extern ApplicationGlobal		gGlobal;
