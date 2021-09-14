#pragma once
#include "../SDK/Includes.hpp"
#include "../Settings.hpp"

class C_Movement
{
public:
	virtual void BunnyHop( );
	virtual void AutoStrafe( );
	virtual void MouseCorrection( );
	virtual void FastStop( );
	virtual void EdgeJump( );
};

inline C_Movement* g_Movement = new C_Movement( );