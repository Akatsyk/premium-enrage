#pragma once
#include "../SDK/Includes.hpp"
#include "LagCompensation.hpp"

enum ADVANCED_ACTIVITY : int
{
	ACTIVITY_NONE = 0,
	ACTIVITY_JUMP,
	ACTIVITY_LAND
};

template < class T >
__forceinline T Interpolate( const T& flCurrent, const T& flTarget, const int iProgress, const int iMaximum )
{
	return flCurrent + ( ( flTarget - flCurrent ) / iMaximum ) * iProgress;
}

class C_AnimationSync
{
public:
	virtual void Instance( ClientFrameStage_t Stage );
	virtual void UpdatePlayerAnimations( C_BasePlayer* pPlayer, C_LagRecord& LagRecord, C_LagRecord PreviousRecord, bool bHasPreviousRecord, int32_t iRotationMode );
	virtual void MarkAsDormant( int32_t iPlayerID ) { m_LeftDormancy[ iPlayerID ] = true; };
	virtual void UnmarkAsDormant( int32_t iPlayerID ) { m_LeftDormancy[ iPlayerID ] = false; };
	virtual bool HasLeftOutOfDormancy( int32_t iPlayerID ) { return m_LeftDormancy[ iPlayerID ]; };
	virtual void SetPreviousRecord( int32_t iPlayerID, C_LagRecord LagRecord ) { m_PreviousRecord[ iPlayerID ] = LagRecord; };
	virtual C_LagRecord GetPreviousRecord( int32_t iPlayerID ) { return m_PreviousRecord[ iPlayerID ]; };
	virtual bool GetCachedMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix );
	virtual void OnUpdateClientSideAnimation( C_BasePlayer* pPlayer );
private:
	std::array < std::array < Vector, MAXSTUDIOBONES >, 65 > m_BoneOrigins;
	std::array < std::array < matrix3x4_t, MAXSTUDIOBONES >, 65 > m_CachedMatrix = { };
	std::array < C_LagRecord, 65 > m_PreviousRecord = { };
	std::array < bool, 65 > m_LeftDormancy = { };
};

inline C_AnimationSync* g_AnimationSync = new C_AnimationSync( );