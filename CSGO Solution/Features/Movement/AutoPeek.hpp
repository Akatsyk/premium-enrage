#pragma once
#include "../SDK/Includes.hpp"

class C_AutoPeek
{
public:
	virtual void Instance( );
	virtual void DrawCircle( );
private:
	float_t m_flAnimationTime = 0.0f;

	Vector m_vecStartPosition = Vector( 0, 0, 0 );

	bool m_bTurnedOn = false;
	bool m_bNegativeSide = false;
	bool m_bRetreat = false;
	bool m_bRetreated = false;
	bool m_bForceDisabled = false;
	bool m_bWaitAnimationProgress = false;
};

inline C_AutoPeek* g_AutoPeek = new C_AutoPeek( );