#pragma once
#include "../SDK/Includes.hpp"

class C_AntiAim
{
public:
	virtual void Instance( );
	virtual float GetTargetYaw( );
	virtual void Micromovement( );
	virtual void SlowWalk( bool bForcedWalk = false );
	virtual void JitterMove( );
	virtual void LegMovement( );
	virtual int32_t GetAntiAimConditions( );
private:
	int32_t m_iDesyncSide = -1;
	int32_t m_iManualSide = 0;
	int32_t m_iFinalSide = -1;
	int32_t m_nFakePickTick = 0;
	int32_t m_nServerTick = 0;

	std::array < int32_t, MULTIPLAYER_BACKUP > m_AntiAimConditions;

	float_t m_flAccelerationSpeed = 95.0f;

	bool m_bSwitch = false;
	bool m_bMoveSwitch = false;
	bool m_bWalkSwitch = false;
	bool m_bSlideSwitch = false;
	bool m_bShouldUseAlternativeSide = false;
};

inline C_AntiAim* g_AntiAim = new C_AntiAim( );