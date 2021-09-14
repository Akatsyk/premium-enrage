#include "LagCompensation.hpp"
#include "Animations.hpp"
#include "../Networking/Networking.hpp"
#include "../Exploits/Exploits.hpp"
#include "../RageBot/RageBot.hpp"

void C_LagCompensation::Instance( ClientFrameStage_t Stage )
{
	if ( Stage != ClientFrameStage_t::FRAME_NET_UPDATE_END )
		return;
	
	for ( int32_t iPlayerID = 1; iPlayerID <= g_Globals.m_Interfaces.m_GlobalVars->m_iMaxClients; iPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( iPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
		{
			g_AnimationSync->MarkAsDormant( iPlayerID );

			g_Globals.m_CachedPlayerRecords[ iPlayerID ].clear( );
			continue;
		}

		if ( pPlayer->IsDormant( ) )
		{
			g_AnimationSync->MarkAsDormant( iPlayerID );
			continue;
		}

		if ( g_AnimationSync->HasLeftOutOfDormancy( iPlayerID ) )
			g_Globals.m_CachedPlayerRecords[ iPlayerID ].clear( );

		if ( pPlayer->m_flOldSimulationTime( ) >= pPlayer->m_flSimulationTime( ) )
		{
			if ( pPlayer->m_flOldSimulationTime( ) > pPlayer->m_flSimulationTime( ) )
				g_Globals.m_CachedPlayerRecords[ iPlayerID ].clear( );

			continue;
		}
		
		bool bHasPreviousRecord = false;
		if ( g_Globals.m_CachedPlayerRecords[ iPlayerID ].empty( ) )
			bHasPreviousRecord = false;
		else if ( TIME_TO_TICKS( fabs( pPlayer->m_flSimulationTime( ) - g_Globals.m_CachedPlayerRecords[ iPlayerID ].back( ).m_SimulationTime ) ) <= 17 )
			bHasPreviousRecord = true;

		C_LagRecord LagRecord;
		this->FillRecord( pPlayer, LagRecord );

		this->UnsetBreakingLagCompensation( iPlayerID );
		if ( bHasPreviousRecord )
		{
			if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flCycle == this->GetPreviousRecord( iPlayerID ).m_AnimationLayers.at( ROTATE_SERVER ).at( ANIMATION_LAYER_ALIVELOOP ).m_flCycle )
				continue;

			g_AnimationSync->SetPreviousRecord( iPlayerID, this->GetPreviousRecord( iPlayerID ) );
			if ( ( LagRecord.m_Origin - this->GetPreviousRecord( iPlayerID ).m_Origin ).Length2DSqr( ) > 4096.0f )
			{
				this->SetBreakingLagCompensation( iPlayerID );
				this->ClearRecords( iPlayerID );
			}
		}

		g_Globals.m_CachedPlayerRecords[ iPlayerID ].emplace_back( LagRecord );
		while ( g_Globals.m_CachedPlayerRecords[ iPlayerID ].size( ) > 32 )
			g_Globals.m_CachedPlayerRecords[ iPlayerID ].pop_front( );
	}
}

void C_LagCompensation::FillRecord( C_BasePlayer* pPlayer, C_LagRecord& LagRecord )
{
	std::memcpy( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), pPlayer->m_AnimationLayers( ), sizeof( C_AnimationLayer ) * ANIMATION_LAYER_COUNT );

	LagRecord.m_UpdateDelay = 1;
	LagRecord.m_Flags = pPlayer->m_fFlags( );
	LagRecord.m_EyeAngles = pPlayer->m_angEyeAngles( );
	LagRecord.m_LowerBodyYaw = pPlayer->m_flLowerBodyYaw( );
	LagRecord.m_Mins = pPlayer->GetCollideable( )->OBBMins( );
	LagRecord.m_Maxs = pPlayer->GetCollideable( )->OBBMaxs( );
	LagRecord.m_SimulationTime = pPlayer->m_flSimulationTime( );
	LagRecord.m_Origin = pPlayer->m_vecOrigin( );
	LagRecord.m_AbsOrigin = pPlayer->GetAbsOrigin( );
	LagRecord.m_DuckAmount = pPlayer->m_flDuckAmount( );
	LagRecord.m_Velocity = pPlayer->m_vecVelocity( );

	if ( pPlayer->m_hActiveWeapon( ).Get( ) )
		if ( pPlayer->m_hActiveWeapon( )->m_flLastShotTime( ) <= pPlayer->m_flSimulationTime( ) )
			if ( pPlayer->m_hActiveWeapon( )->m_flLastShotTime( ) > pPlayer->m_flOldSimulationTime( ) )
				LagRecord.m_bIsShooting = true;
}

void C_LagCompensation::ResetData( )
{
	for ( int32_t iPlayerID = 0; iPlayerID < 65; iPlayerID++ )
	{
		if ( !g_Globals.m_CachedPlayerRecords[ iPlayerID ].empty( ) )
			g_Globals.m_CachedPlayerRecords[ iPlayerID ].clear( );

		m_BreakingLagcompensation[ iPlayerID ] = false;	
	}
}

void C_LagCompensation::InterpolatePlayers( )
{
	for ( int32_t iPlayerID = 1; iPlayerID <= g_Globals.m_Interfaces.m_GlobalVars->m_iMaxClients; iPlayerID++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( iPlayerID );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) || pPlayer->IsDormant( ) )
			continue;
		
		auto v27 = *( DWORD* )( ( DWORD )( pPlayer ) + 0x24 );
        *( short* )( v27 + 0xE ) = 0;
        *( DWORD* )( *( DWORD* )( v27 + 0x14 ) + 0x24 ) = 0;
        auto v29 = *( DWORD* )( ( DWORD )( pPlayer ) + 0x24 );
        *( short* )( v29 + 0x1A ) = 0;
        *( DWORD* )( *( DWORD* )( v29 + 0x20 ) + 0x24 ) = 0;
        auto v30 = *( DWORD* )( ( DWORD )( pPlayer ) + 0x24 );
        *( short* )( v30 + 0x26 ) = 0;
        *( DWORD* )( *( DWORD* )( v30 + 0x2C ) + 0x24 ) = 0;
        auto v31 = *( DWORD* )( ( DWORD )( pPlayer ) + 0x24 );
        *( short* )( v31 + 0x32 ) = 0;
        *( DWORD* )( *( DWORD* )( v31 + 0x38 ) + 0x24 ) = 0;

        *( float* )( *( DWORD* )( *( DWORD* )( ( DWORD )( pPlayer ) + 0x24 ) + 8 ) + 0x24 ) = 14.0f / g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
        *( float* )( *( DWORD* )( *( DWORD* )( ( DWORD )( pPlayer ) + 0x24 ) + 0x44 ) + 0x24 ) = 14.0f / g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
	}
}

bool C_LagCompensation::IsValidTime( float_t flTime )
{
	int32_t nTickbase = g_Globals.m_LocalPlayer->m_nTickBase( );
	if ( nTickbase <= 0 )
		return false;
	
	float_t flDeltaTime = fmin( g_Networking->GetLatency( ) + this->GetLerpTime( ), 0.2f ) - TICKS_TO_TIME( nTickbase - TIME_TO_TICKS( flTime ) );
	if ( fabs( flDeltaTime ) > 0.2f )
		return false;
		
	int32_t nDeadTime = TICKS_TO_TIME( g_Networking->GetServerTick( ) ) - 0.2f;
	if ( TIME_TO_TICKS( flTime + this->GetLerpTime( ) ) < nDeadTime )
		return false;

	return true;
}

float_t C_LagCompensation::GetLerpTime( )
{
	float_t flUpdateRate = std::clamp( g_Globals.m_ConVars.m_ClUpdateRate->GetFloat( ), g_Globals.m_ConVars.m_SvMinUpdateRate->GetFloat( ), g_Globals.m_ConVars.m_SvMaxUpdateRate->GetFloat( ) );
	float_t flLerpRatio = std::clamp( g_Globals.m_ConVars.m_ClInterpRatio->GetFloat( ), g_Globals.m_ConVars.m_SvClientMinInterpRatio->GetFloat( ), g_Globals.m_ConVars.m_SvClientMaxInterpRatio->GetFloat( ) );
	return std::clamp( flLerpRatio / flUpdateRate, g_Globals.m_ConVars.m_ClInterp->GetFloat( ), 1.0f );
}