#pragma once
#include "../SDK/Includes.hpp"

struct PenetrationData_t
{
	Vector m_vecShootPosition = Vector( 0, 0, 0 );
	Vector m_vecTargetPosition = Vector( 0, 0, 0 );
	Vector m_vecDirection = Vector( 0, 0, 0 );

	int32_t m_PenetrationCount = 4;

	float_t m_flPenetrationPower = 0.0f;
	float_t m_flPenetrationDistance = 0.0f;

	float_t m_flDamageModifier = 0.5f;
	float_t m_flPenetrationModifier = 1.0f;

	float_t m_flMaxRange = 0.0f;
	float_t m_flWeaponDamage = 0.0f;
	float_t m_flCurrentDamage = 0.0f;
	float_t m_flCurrentDistance = 0.0f;

	CGameTrace m_EnterTrace;

	C_CSWeaponData* m_WeaponData = NULL;
	C_BaseCombatWeapon* m_Weapon = NULL;
};

class C_AutoWall
{
public:
	virtual bool TraceToExit( Vector vecStart, Vector& vecEnd, CGameTrace* pEnterTrace, CGameTrace* pExitTrace );
	virtual bool HandleBulletPenetration( );
	virtual bool IsArmored( C_BasePlayer* pPlayer, int32_t nHitGroup );
	virtual void ScaleDamage( CGameTrace Trace, float_t& flDamage );
	virtual void ClipTraceToPlayers( C_BasePlayer* pPlayer, Vector vecStart, Vector vecEnd, CGameTrace* Trace, CTraceFilter* Filter, uint32_t nMask );
	virtual Vector GetPointDirection( Vector vecShootPosition, Vector vecTargetPosition );
	virtual float GetPointDamage( Vector vecShootPosition, Vector vecTargetPosition );
	virtual bool SimulateFireBullet( );
	virtual void CacheWeaponData( );
private:
	PenetrationData_t m_PenetrationData;
};

inline C_AutoWall* g_AutoWall = new C_AutoWall( );