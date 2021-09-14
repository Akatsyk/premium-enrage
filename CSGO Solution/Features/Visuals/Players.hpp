#pragma once
#include "../SDK/Includes.hpp"
#include "../Settings.hpp"

struct C_PlayerVisualsData
{
	C_PlayerSettings m_aSettings;
	RECT m_aBBox;

	int32_t m_iHeight = 0;
	int32_t m_iWidth = 0;

	float_t m_flPreviousHealth = 100;
	float_t m_flPreviousAmmo = 20;

	bool m_bLeftDormancy = false;
};

struct DormantPlayer_t
{
	float_t m_ReceiveTime = 0.0f;
	float_t m_InDormant = 0.0f;
	Vector m_Origin = Vector( 0, 0, 0 );
	Vector m_LastOrigin = Vector( 0, 0, 0 );
};

class C_PlayerESP
{
public:
	virtual void Instance( );
	virtual void RenderName( C_BasePlayer* player );
	virtual void RenderHealth( C_BasePlayer* player );
	virtual void RenderWeapon( C_BasePlayer* player );
	virtual void RenderBox( C_BasePlayer* player );
	virtual void RenderAmmo( C_BasePlayer* player );
	virtual void RenderFlags( C_BasePlayer* player );
	virtual std::string GetWeaponName( C_BaseCombatWeapon* pWeapon );
	virtual std::string GetWeaponIcon( C_BasePlayer* pPlayer, C_BaseCombatWeapon* pWeapon );

	virtual void RenderOutOfView( C_BasePlayer* pPlayer );
	virtual void RenderGlow();
	virtual bool ForceDormant( int nIndex );
	virtual void DormantPlayers( );
	virtual bool IsSoundValid( SndInfo_t Sound );
	virtual void ResetData( );

	std::array < C_PlayerVisualsData, 65 > m_aPlayerData;
private:
	std::array < DormantPlayer_t, 65 > m_DormantPlayers;
	CUtlVector < SndInfo_t > m_CurrentSoundData;
	CUtlVector < SndInfo_t > m_CachedSoundData;
};

inline C_PlayerESP* g_PlayerESP = new C_PlayerESP( );