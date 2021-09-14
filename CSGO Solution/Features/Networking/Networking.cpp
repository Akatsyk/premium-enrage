#include "Networking.hpp"
#include "../Exploits/Exploits.hpp"
#include "../Log Manager/LogManager.hpp"

inline bool IsVectorValid( Vector vecOriginal, Vector vecCurrent )
{
	Vector vecDelta = vecOriginal - vecCurrent;
	for ( int i = 0; i < 3; i++ )
	{
		if ( fabsf( vecDelta[ i ] ) > 0.03125f )
			return false;
	}

	return true;
}

inline bool IsAngleValid( QAngle vecOriginal, QAngle vecCurrent )
{
	QAngle vecDelta = vecOriginal - vecCurrent;
	for ( int i = 0; i < 3; i++ )
	{
		if ( fabsf( vecDelta[ i ] ) > 0.03125f )
			return false;
	}

	return true;
}

inline bool IsFloatValid( float_t flOriginal, float_t flCurrent )
{
	if ( fabsf( flOriginal - flCurrent ) > 0.03125f )
		return false;

	return true;
}

void C_Networking::OnPacketEnd( C_ClientState* m_ClientState )
{
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
		return;

	if ( *( int32_t* )( ( DWORD )( m_ClientState ) + 0x16C ) != *( int32_t* )( ( DWORD )( m_ClientState ) + 0x164 ) )
		return;

	g_PredictionSystem->AdjustViewmodelData( );
	return this->RestoreNetvarData( g_Globals.m_Interfaces.m_ClientState->m_nLastCommandAck( ) );
}

void C_Networking::ProcessInterpolation( ClientFrameStage_t Stage, bool bPostFrame )
{
	if ( Stage != ClientFrameStage_t::FRAME_RENDER_START )
		return;

	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
		return;

	if ( !bPostFrame )
	{
		m_FinalPredictedTick = g_Globals.m_LocalPlayer->m_nFinalPredictedTick( );
		
		g_Globals.m_LocalPlayer->m_nFinalPredictedTick( ) = g_Globals.m_Interfaces.m_GlobalVars->m_iTickCount + TIME_TO_TICKS( m_Latency );
		return;
	}

	g_Globals.m_LocalPlayer->m_nFinalPredictedTick( ) = m_FinalPredictedTick;
}

void C_Networking::SaveNetvarData( int nCommand )
{
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_nTickbase				= g_Globals.m_LocalPlayer->m_nTickBase( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckAmount				= g_Globals.m_LocalPlayer->m_flDuckAmount( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckSpeed				= g_Globals.m_LocalPlayer->m_flDuckSpeed( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecOrigin				= g_Globals.m_LocalPlayer->m_vecOrigin( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecVelocity				= g_Globals.m_LocalPlayer->m_vecVelocity( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecBaseVelocity			= g_Globals.m_LocalPlayer->m_vecBaseVelocity( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flFallVelocity			= g_Globals.m_LocalPlayer->m_flFallVelocity( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecViewOffset			= g_Globals.m_LocalPlayer->m_vecViewOffset( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_angAimPunchAngle			= g_Globals.m_LocalPlayer->m_aimPunchAngle( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_vecAimPunchAngleVel		= g_Globals.m_LocalPlayer->m_aimPunchAngleVel( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_angViewPunchAngle		= g_Globals.m_LocalPlayer->m_viewPunchAngle( );

    C_BaseCombatWeapon* pWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
    if ( !pWeapon )
	{
		m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flRecoilIndex = 0.0f;
		m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flAccuracyPenalty = 0.0f;
		m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flPostponeFireReadyTime = 0.0f;

		return;
	}

	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flRecoilIndex = pWeapon->m_flRecoilIndex( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flAccuracyPenalty = pWeapon->m_fAccuracyPenalty( );
	m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ].m_flPostponeFireReadyTime = pWeapon->m_flPostponeFireReadyTime( );
}

void C_Networking::RestoreNetvarData( int nCommand )
{
	volatile auto aNetVars = &m_aCompressData[ nCommand % MULTIPLAYER_BACKUP ];
	if ( aNetVars->m_nTickbase != g_Globals.m_LocalPlayer->m_nTickBase( ) )
		return;

	if ( IsVectorValid( aNetVars->m_vecVelocity, g_Globals.m_LocalPlayer->m_vecVelocity( ) ) )
		g_Globals.m_LocalPlayer->m_vecVelocity( )							=	aNetVars->m_vecVelocity;

	if ( IsVectorValid( aNetVars->m_vecBaseVelocity, g_Globals.m_LocalPlayer->m_vecBaseVelocity( ) ) )
		g_Globals.m_LocalPlayer->m_vecBaseVelocity( )						=	aNetVars->m_vecBaseVelocity;
	
	if ( IsVectorValid( aNetVars->m_vecViewOffset, g_Globals.m_LocalPlayer->m_vecViewOffset( ) ) )
		g_Globals.m_LocalPlayer->m_vecViewOffset( )							=	aNetVars->m_vecViewOffset;

	if ( IsAngleValid( aNetVars->m_angAimPunchAngle, g_Globals.m_LocalPlayer->m_aimPunchAngle( ) ) )
		g_Globals.m_LocalPlayer->m_aimPunchAngle( )							=	aNetVars->m_angAimPunchAngle;
		
	if ( IsVectorValid( aNetVars->m_vecAimPunchAngleVel, g_Globals.m_LocalPlayer->m_aimPunchAngleVel( ) ) )
		g_Globals.m_LocalPlayer->m_aimPunchAngleVel( )						=	aNetVars->m_vecAimPunchAngleVel;

	if ( IsAngleValid( aNetVars->m_angViewPunchAngle, g_Globals.m_LocalPlayer->m_viewPunchAngle( ) ) )
		g_Globals.m_LocalPlayer->m_viewPunchAngle( )						=	aNetVars->m_angViewPunchAngle;

	if ( IsFloatValid( aNetVars->m_flFallVelocity, g_Globals.m_LocalPlayer->m_flFallVelocity( ) ) )
		g_Globals.m_LocalPlayer->m_flFallVelocity( )						=	aNetVars->m_flFallVelocity;

	if ( IsFloatValid( aNetVars->m_flDuckAmount, g_Globals.m_LocalPlayer->m_flDuckAmount( ) ) )
		g_Globals.m_LocalPlayer->m_flDuckAmount( )							=	aNetVars->m_flDuckAmount;

	if ( IsFloatValid( aNetVars->m_flDuckSpeed, g_Globals.m_LocalPlayer->m_flDuckSpeed( ) ) )
		g_Globals.m_LocalPlayer->m_flDuckSpeed( )							=	aNetVars->m_flDuckSpeed;

	if ( g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( ) )
	{
		if ( IsFloatValid( aNetVars->m_flAccuracyPenalty, g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_fAccuracyPenalty( ) ) )
			g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_fAccuracyPenalty( ) = aNetVars->m_flAccuracyPenalty;
		
		if ( IsFloatValid( aNetVars->m_flRecoilIndex, g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_flRecoilIndex( ) ) )
			g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_flRecoilIndex( ) = aNetVars->m_flRecoilIndex;
	}

	if ( g_Globals.m_LocalPlayer->m_vecViewOffset( ).z > 64.0f )
		g_Globals.m_LocalPlayer->m_vecViewOffset( ).z = 64.0f;
	else if ( g_Globals.m_LocalPlayer->m_vecViewOffset( ).z <= 46.05f )
		g_Globals.m_LocalPlayer->m_vecViewOffset( ).z = 46.0f;
	
	if ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND )
		g_Globals.m_LocalPlayer->m_flFallVelocity( ) = 0.0f;
}

void C_Networking::UpdateLatency( )
{
	C_NetChannelInfo* pNetChannelInfo = g_Globals.m_Interfaces.m_EngineClient->GetNetChannelInfo( );
	if ( !pNetChannelInfo )
		return;

	m_Latency = pNetChannelInfo->GetLatency( FLOW_OUTGOING ) + pNetChannelInfo->GetLatency( FLOW_INCOMING );
}

int32_t C_Networking::GetServerTick( )
{	
	int32_t nExtraChoke = 0;
	if ( g_Globals.m_Packet.m_bFakeDuck )
		nExtraChoke = 14 - g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( );

	return g_Globals.m_Interfaces.m_GlobalVars->m_iTickCount + TIME_TO_TICKS( m_Latency ) + nExtraChoke - g_ExploitSystem->GetShiftAmount( );
}

int32_t C_Networking::GetTickRate( )
{
	return ( int32_t )( 1.0f / g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick );
}

float_t C_Networking::GetLatency( )
{
	return m_Latency;
}

void C_Networking::ResetData( )
{
	m_Latency = 0.0f;
	m_TickRate = 0;

	m_aCompressData = { };
}