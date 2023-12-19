#pragma once
#include "appframe.h"



class ApplicationMain : public ApplicationBase
{
	typedef ApplicationBase base;
public:
	virtual bool Initialize(HINSTANCE hInstance);
	virtual bool Terminate();
	virtual bool Input();
	virtual bool Process();
	virtual bool Render();

	virtual bool AppWindowed() { return true; }
#ifdef _DEBUG 
	virtual int DispSizeW() { return 1280; }
	virtual int DispSizeH() { return 720; }
#else
	virtual int DispSizeW() { return 1920; }
	virtual int DispSizeH() { return 1080; }
#endif
protected:

}; 
