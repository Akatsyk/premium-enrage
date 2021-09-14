#pragma once
#include <vector>
#include "../SDK/Includes.hpp"

struct ClientImpact_t
{
	Vector m_vecPosition;
	float_t m_flTime;
	float_t m_flExpirationTime;
};

struct C_BulletTrace
{
	bool m_bIsLocalTrace = false;

	Vector m_vecStartPosition = Vector( 0, 0, 0 );
	Vector m_vecEndPosition = Vector( 0, 0, 0 );
};

class C_World
{
public:
	virtual void Instance( ClientFrameStage_t Stage );
	virtual void SkyboxChanger( );
	virtual void DrawClientImpacts( );
	virtual void DrawBulletTracers( );
	virtual void OnBulletImpact( C_GameEvent* pEvent );
	virtual void Grenades( );
	virtual void DrawScopeLines( );
	virtual void RemoveShadows( );
	virtual void RemoveHandShaking( );
	virtual void RemoveSmokeAndPostProcess( );
	virtual void PostFrame( ClientFrameStage_t Stage );
	virtual void OnRageBotFire( Vector vecStartPosition, Vector vecEndPosition );
	virtual void PreserveKillfeed( );

	std::vector < C_BulletTrace > m_BulletTracers = { };

private:
	int32_t m_iLastProcessedImpact = 0;

	bool m_bDidUnlockConvars = false;
};

inline C_World* g_World = new C_World( );