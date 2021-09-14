#pragma once
#include <deque>
#include "../SDK/Includes.hpp"

class C_LagCompensation
{
public:
	virtual void Instance( ClientFrameStage_t Stage );
	virtual void FillRecord( C_BasePlayer* Player, C_LagRecord& Record );
	virtual void InterpolatePlayers( );
	virtual void ResetData( );
	virtual void ClearRecords( int32_t iPlayerID ) { g_Globals.m_CachedPlayerRecords[ iPlayerID ].clear( ); };
	virtual void SetBreakingLagCompensation( int32_t iPlayerID ) { m_BreakingLagcompensation[ iPlayerID ] = true; };
	virtual void UnsetBreakingLagCompensation( int32_t iPlayerID ) { m_BreakingLagcompensation[ iPlayerID ] = false; };
	virtual bool IsValidTime( float_t flTime );
	virtual bool IsBreakingLagCompensation( int32_t iPlayerID ) { return m_BreakingLagcompensation[ iPlayerID ]; };
	virtual float_t GetLerpTime( );
	virtual std::deque < C_LagRecord >& GetPlayerRecords( int32_t iPlayerID ) { return g_Globals.m_CachedPlayerRecords[ iPlayerID ]; };
	virtual C_LagRecord& GetPreviousRecord( int32_t iPlayerID ) { return g_Globals.m_CachedPlayerRecords[ iPlayerID ].back( ); };
private:
	std::array < bool, 65 > m_BreakingLagcompensation = { };
};

inline C_LagCompensation* g_LagCompensation = new C_LagCompensation( );