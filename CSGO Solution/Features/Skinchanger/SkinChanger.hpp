#pragma once
#include "../SDK/Includes.hpp"

class C_SkinChanger
{
public:
	virtual void Instance( ClientFrameStage_t Stage );
};

inline C_SkinChanger* g_SkinChanger = new C_SkinChanger( );