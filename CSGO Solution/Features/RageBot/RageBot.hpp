#pragma once
#include <deque>
#include "../SDK/Includes.hpp"
#include "../Settings.hpp"

#include "../Animations/LagCompensation.hpp"

class C_RageBot
{
public:
	virtual void Instance( );
	virtual void SetupPacket( );
	virtual void AutoRevolver( );
	virtual int32_t GetMinDamage( C_BasePlayer* pPlayer );
	virtual int32_t GetAutoDamage( bool bHasArmour );
	virtual void FakeDuck( );
	virtual void AutoStop( );
	virtual void AutoScope( );
	virtual void ScanPlayers( );
	virtual void TaserBot( );

	virtual void BackupPlayers( );
	virtual void RestorePlayers( );

	virtual void ResetData( );
	virtual void SaveMovementData( );
	virtual void ForceMovementData( );
	virtual void AdjustRevolverData( int32_t nCommand, int32_t nButtons );

	virtual void OnWeaponFire( C_GameEvent* pEvent );
	virtual void OnBulletImpact( C_GameEvent* pEvent );
	virtual void OnPlayerHurt( C_GameEvent* pEvent );
	virtual void OnNetworkUpdate( ClientFrameStage_t Stage );
	
	virtual C_RageSettings GetRageSettings( ) { return this->m_RageSettings; };
	static int GetHitgroupFromHitbox( int32_t iHitbox );

	virtual bool CanAutoStop( );
	virtual bool HasMaximumAccuracy( );
	virtual void PredictAutoStop( );
	virtual void UpdatePeekState( );

	virtual C_HitboxData ScanPlayerRecord( C_BasePlayer* pPlayer, C_LagRecord LagRecord, Vector vecStartPosition );
	virtual C_LagRecord GetFirstAvailableRecord( C_BasePlayer* pPlayer );
	virtual bool FindPlayerRecord( C_BasePlayer* pPlayer, C_LagRecord* OutRecord, C_HitboxData* OutHitbox );
	virtual std::vector < Vector > GetHitboxPoints( C_BasePlayer* pPlayer, C_LagRecord LagRecord, Vector vecStartPosition, int32_t iHitbox );
	virtual bool DoesIntersectHitbox( C_BasePlayer* pPlayer, int32_t iHitbox, Vector vecStartPosition, Vector vecEndPosition );
	virtual bool IsSafePoint( C_BasePlayer* pPlayer, C_LagRecord LagRecord, Vector vecStartPosition, Vector vecEndPosition, int32_t iHitbox );
	virtual void AdjustPlayerBones( C_BasePlayer* pPlayer, std::array < matrix3x4_t, MAXSTUDIOBONES > aMatrix );
	virtual void AdjustPlayerRecord( C_BasePlayer* pPlayer, C_LagRecord LagRecord );
	virtual float_t GetHitChance( C_TargetData Target );
	virtual float_t GetAccuracyBoost( C_TargetData Target );
private:
	C_RageSettings m_RageSettings = C_RageSettings( );
	C_MoveData m_MoveData = C_MoveData( );

	float_t m_flSideMove = 0.0f;
	float_t m_flForwardMove = 0.0f;

	std::array < C_LagRecord, 65 > m_BackupData;
};

inline C_RageBot* g_RageBot = new C_RageBot( );