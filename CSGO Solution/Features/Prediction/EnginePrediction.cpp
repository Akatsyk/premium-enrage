#include "EnginePrediction.hpp"
#include "../Packet/PacketManager.hpp"

void C_PredictionSystem::Instance( )
{
	m_flOldCurtime = g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime;
	m_flOldFrametime = g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime;
	
	m_bInPrediction_Backup = g_Globals.m_Interfaces.m_Prediction->m_bInPrediction( );
	m_bIsFirstTimePredicted_Backup = g_Globals.m_Interfaces.m_Prediction->m_bIsFirstTimePredicted( );
	
	if ( !m_LastPredictedCmd || m_LastPredictedCmd->m_bHasBeenPredicted )
		m_nTickBase++;
	else
		m_nTickBase = g_Globals.m_LocalPlayer->m_nTickBase( );

	g_Globals.m_Interfaces.m_Prediction->m_bInPrediction( ) = true;
	g_Globals.m_Interfaces.m_Prediction->m_bIsFirstTimePredicted( ) = false;

	return g_PredictionSystem->Repredict( );
}

void C_PredictionSystem::Repredict( )
{
	g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime = m_nTickBase * g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;
	if ( !( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_FROZEN ) )
		g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime = g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick;

	float_t flRecoilIndex = 0.0f;
	float_t flAccuracyPenalty = 0.0f;
	
	Vector vecPreviousOrigin = g_Globals.m_LocalPlayer->GetAbsOrigin( );
	if ( g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( ) )
	{
		flRecoilIndex = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_flRecoilIndex( );
		flAccuracyPenalty = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_fAccuracyPenalty( );
	}

	g_Globals.m_LocalPlayer->m_vecAbsVelocity( ) = g_Globals.m_LocalPlayer->m_vecVelocity( );
	g_Globals.m_LocalPlayer->SetAbsoluteOrigin( g_Globals.m_LocalPlayer->m_vecOrigin( ) );

	*( C_UserCmd** )( ( uintptr_t )( g_Globals.m_LocalPlayer ) + 0x3338 ) = g_PacketManager->GetModifableCommand( );
	( **( ( C_BasePlayer*** )( g_Globals.m_AddressList.m_PredictionPlayer ) ) ) = g_Globals.m_LocalPlayer;
	( *( *( int32_t** )( g_Globals.m_AddressList.m_PredictionSeed ) ) ) = g_PacketManager->GetModifableCommand( )->m_iRandomSeed;
	
	g_Globals.m_Interfaces.m_GameMovement->StartTrackPredictionErrors( g_Globals.m_LocalPlayer );
	g_Globals.m_Interfaces.m_MoveHelper->SetHost( g_Globals.m_LocalPlayer );

	g_Globals.m_Interfaces.m_Prediction->SetupMove( g_Globals.m_LocalPlayer, g_PacketManager->GetModifableCommand( ), g_Globals.m_Interfaces.m_MoveHelper, &m_MoveData );
	g_Globals.m_Interfaces.m_GameMovement->SetupMovementBounds( &m_MoveData );
	g_Globals.m_Interfaces.m_GameMovement->ProcessMovement( g_Globals.m_LocalPlayer, &m_MoveData );
	g_Globals.m_Interfaces.m_Prediction->FinishMove( g_Globals.m_LocalPlayer, g_PacketManager->GetModifableCommand( ), &m_MoveData );
	
	g_Globals.m_Interfaces.m_MoveHelper->SetHost( nullptr );
	g_Globals.m_Interfaces.m_GameMovement->FinishTrackPredictionErrors( g_Globals.m_LocalPlayer );

	*( C_UserCmd** )( ( uintptr_t )( g_Globals.m_LocalPlayer ) + 0x3338 ) = NULL;
	( **( ( C_BasePlayer*** )( g_Globals.m_AddressList.m_PredictionPlayer ) ) ) = NULL;
	( *( *( int32_t** )( g_Globals.m_AddressList.m_PredictionSeed ) ) ) = -1;

	m_LastPredictedCmd = g_PacketManager->GetModifableCommand( );
	if ( g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( ) )
	{
		g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_flRecoilIndex( ) = flRecoilIndex;
		g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->m_fAccuracyPenalty( ) = flAccuracyPenalty;
		g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( )->UpdateAccuracyPenalty( );
	}

	return g_Globals.m_LocalPlayer->SetAbsoluteOrigin( vecPreviousOrigin );
}

void C_PredictionSystem::ResetData( )
{
	m_aNetvarData = { };
	
	m_flOldCurtime = 0.0f;
	m_flOldFrametime = 0.0f;
	m_flVelocityModifier = 1.0f;
	m_iLastCommand = -1;
	m_bInPrediction_Backup = false;
	m_bIsFirstTimePredicted_Backup = false;
	m_LastPredictedCmd = NULL;
	m_MoveData = C_MoveData( );
	m_nTickBase = 0;
}

void C_PredictionSystem::SaveVelocityModifier( )
{
	m_flVelocityModifier = g_Globals.m_LocalPlayer->m_flVelocityModifier( );
}

void C_PredictionSystem::SaveCommand( int32_t nCommand )
{
	m_iLastCommand = nCommand;
}

float_t C_PredictionSystem::GetVelocityModifier( int32_t nCommand )
{
	float_t flVelocityModifier = m_flVelocityModifier + ( min( ( ( nCommand - 1 ) - m_iLastCommand ), 1 ) * ( g_Globals.m_Interfaces.m_GlobalVars->m_flIntervalPerTick * 0.4f ) );
	if ( flVelocityModifier > 1.0f )
		return 1.0f;

	if ( !( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
		return g_Globals.m_LocalPlayer->m_flVelocityModifier( );

	return flVelocityModifier;
}

void C_PredictionSystem::SaveNetvars( int32_t nCommand )
{
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_fFlags						= g_Globals.m_LocalPlayer->m_fFlags( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_hGroundEntity				= g_Globals.m_LocalPlayer->m_hGroundEntity( ).Get( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckAmount				= g_Globals.m_LocalPlayer->m_flDuckAmount( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckSpeed				= g_Globals.m_LocalPlayer->m_flDuckSpeed( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecOrigin					= g_Globals.m_LocalPlayer->m_vecOrigin( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecVelocity				= g_Globals.m_LocalPlayer->m_vecVelocity( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecBaseVelocity			= g_Globals.m_LocalPlayer->m_vecBaseVelocity( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flFallVelocity				= g_Globals.m_LocalPlayer->m_flFallVelocity( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecViewOffset				= g_Globals.m_LocalPlayer->m_vecViewOffset( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_angAimPunchAngle			= g_Globals.m_LocalPlayer->m_aimPunchAngle( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecAimPunchAngleVel		= g_Globals.m_LocalPlayer->m_aimPunchAngleVel( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_angViewPunchAngle			= g_Globals.m_LocalPlayer->m_viewPunchAngle( );

    C_BaseCombatWeapon* pWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
    if ( !pWeapon )
		return;

	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flRecoilIndex = pWeapon->m_flRecoilIndex( );
	m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flAccuracyPenalty = pWeapon->m_fAccuracyPenalty( );
}

void C_PredictionSystem::RestoreNetvars( int32_t nCommand )
{
	g_Globals.m_LocalPlayer->m_fFlags( )							= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_fFlags;
    g_Globals.m_LocalPlayer->m_hGroundEntity( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_hGroundEntity.Get( );
    g_Globals.m_LocalPlayer->m_flDuckAmount( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckAmount;
    g_Globals.m_LocalPlayer->m_flDuckSpeed( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flDuckSpeed;
    g_Globals.m_LocalPlayer->m_vecOrigin( )							= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecOrigin;
    g_Globals.m_LocalPlayer->m_vecVelocity( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecVelocity;
    g_Globals.m_LocalPlayer->m_vecBaseVelocity( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecBaseVelocity;
    g_Globals.m_LocalPlayer->m_flFallVelocity( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flFallVelocity;
    g_Globals.m_LocalPlayer->m_vecViewOffset( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecViewOffset;
    g_Globals.m_LocalPlayer->m_aimPunchAngle( )						= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_angAimPunchAngle;
    g_Globals.m_LocalPlayer->m_aimPunchAngleVel( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_vecAimPunchAngleVel;
    g_Globals.m_LocalPlayer->m_viewPunchAngle( )					= m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_angViewPunchAngle;

    C_BaseCombatWeapon* pWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
    if ( !pWeapon )
        return;
    
    pWeapon->m_flRecoilIndex( ) = m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flRecoilIndex;
    pWeapon->m_fAccuracyPenalty( ) = m_aNetvarData[ nCommand % MULTIPLAYER_BACKUP ].m_flAccuracyPenalty;
}

void C_PredictionSystem::ResetPacket( )
{
	g_Globals.m_Interfaces.m_GlobalVars->m_flCurTime = m_flOldCurtime;
	g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime = m_flOldFrametime;

	g_Globals.m_Interfaces.m_Prediction->m_bInPrediction( ) = m_bInPrediction_Backup;
	g_Globals.m_Interfaces.m_Prediction->m_bIsFirstTimePredicted( ) = m_bIsFirstTimePredicted_Backup;
}

void C_PredictionSystem::UpdatePacket( )
{
	if ( g_Globals.m_Interfaces.m_ClientState->m_nDeltaTick( ) < 0 )
		return;

	return g_Globals.m_Interfaces.m_Prediction->Update( g_Globals.m_Interfaces.m_ClientState->m_nDeltaTick( ), g_Globals.m_Interfaces.m_ClientState->m_nDeltaTick( ) > 0, g_Globals.m_Interfaces.m_ClientState->m_nLastCommandAck( ), g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) + g_Globals.m_Interfaces.m_ClientState->m_nLastOutgoingCommand( ) );
}

void C_PredictionSystem::SaveViewmodelData( )
{
	C_BaseViewModel* hViewmodel = g_Globals.m_LocalPlayer->m_hViewModel( ).Get( );
	if ( !hViewmodel )
		return;

	m_iAnimationParity = hViewmodel->m_iAnimationParity( );
	m_iSequence = hViewmodel->m_iSequence( );
	m_flCycle = hViewmodel->m_flCycle( );
	m_flAnimTime = hViewmodel->m_flAnimTime( );
}

void C_PredictionSystem::AdjustViewmodelData( )
{
	C_BaseViewModel* hViewmodel = g_Globals.m_LocalPlayer->m_hViewModel( ).Get( );
	if ( !hViewmodel )
		return;

	if ( m_iSequence != hViewmodel->m_iSequence( ) || m_iAnimationParity != hViewmodel->m_iAnimationParity( ) )
		return;

	hViewmodel->m_flCycle( ) = m_flCycle;
	hViewmodel->m_flAnimTime( ) = m_flAnimTime;
}