#include "../Hooks.hpp"
#include "../Menu.hpp"
#include "../Settings.hpp"
#include "../Features/Packet/PacketManager.hpp"
#include "../Features/Movement/Movement.hpp"
#include "../Features/Prediction/EnginePrediction.hpp"
#include "../Features/Animations/LagCompensation.hpp"
#include "../Features/RageBot/RageBot.hpp"
#include "../Features/RageBot/Autowall.hpp"
#include "../Features/RageBot/Antiaim.hpp"
#include "../Features/Networking/Networking.hpp"
#include "../Features/Networking/Fakelag.hpp"
#include "../Features/Animations/LocalAnimations.hpp"
#include "../Features/Animations/Animations.hpp"
#include "../Features/Visuals/World.hpp"
#include "../Features/Weather/Weather.hpp"
#include "../Features/Grenades/Warning.hpp"
#include "../Features/Exploits/Exploits.hpp"
#include "../Features/Visuals/Players.hpp"

void __fastcall C_Hooks::hkFrameStageNotify( LPVOID pEcx, uint32_t, ClientFrameStage_t Stage )
{
	g_Globals.m_LocalPlayer = C_BasePlayer::GetPlayerByIndex( g_Globals.m_Interfaces.m_EngineClient->GetLocalPlayer( ) );
	if ( !g_Globals.m_LocalPlayer )
	{
		g_LagCompensation->ResetData( );
		g_Networking->ResetData( );
		g_LocalAnimations->ResetData( );
		g_PredictionSystem->ResetData( );
		g_RageBot->ResetData( );
		g_PlayerESP->ResetData( );
		g_FakeLag->ResetData( );

		return g_Globals.m_Hooks.m_Originals.m_FrameStageNotify( Stage );
	}

	g_World->Instance( Stage );
	
	g_LagCompensation->Instance( Stage );
	g_RageBot->OnNetworkUpdate( Stage );
	g_AnimationSync->Instance( Stage );

	g_Networking->ProcessInterpolation( Stage, false );
	g_Globals.m_Hooks.m_Originals.m_FrameStageNotify( Stage );
	g_Networking->ProcessInterpolation( Stage, true );

	return g_World->PostFrame( Stage );
}

void __stdcall C_Hooks::hkCreateMove( int32_t iSequence, float_t flFrametime, bool bIsActive, bool& bSendPacket )
{
	g_Globals.m_Hooks.m_Originals.m_CreateMove( iSequence, flFrametime, bIsActive );
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) || !g_PacketManager->SetupPacket( iSequence, &bSendPacket ) )
		return g_Globals.m_Hooks.m_Originals.m_CreateMove( iSequence, flFrametime, bIsActive );

	g_Globals.m_Packet.m_bInCreateMove = true;

	g_ExploitSystem->SetupCommand( );
	if ( g_Menu->IsMenuOpened( ) )
		g_PacketManager->GetModifableCommand( )->m_nButtons &= ~( IN_ATTACK | IN_ATTACK2 | IN_RELOAD );

	g_Globals.m_AccuracyData.m_bCanFire_Default = g_Globals.m_LocalPlayer->CanFire( );
	g_Globals.m_AccuracyData.m_bCanFire_Shift = g_Globals.m_AccuracyData.m_bCanFire_Default;
	if ( g_ExploitSystem->GetActiveExploit( ) == HIDESHOTS )
		g_Globals.m_AccuracyData.m_bCanFire_Shift = g_Globals.m_LocalPlayer->CanFire( g_ExploitSystem->GetShiftAmount( ) );

	g_PredictionSystem->UpdatePacket( );
	g_PredictionSystem->SaveNetvars( g_PacketManager->GetModifableCommand( )->m_nCommand );

	g_Movement->BunnyHop( );
	g_Movement->MouseCorrection( );
	g_Movement->AutoStrafe( );
	g_Movement->FastStop( );

	g_AntiAim->SlowWalk( );
	g_AntiAim->JitterMove( );
	g_AntiAim->Micromovement( );
	g_AntiAim->LegMovement( );
	
	g_RageBot->BackupPlayers( );
	g_AutoWall->CacheWeaponData( );

	if ( !g_ExploitSystem->PerformCommand( ) )
	{
		C_VerifiedUserCmd* pVerifiedCmd = g_Globals.m_Interfaces.m_Input->GetVerifiedCmd( iSequence );
		if ( pVerifiedCmd )
		{
			pVerifiedCmd->m_Cmd = *g_PacketManager->GetModifableCommand( );
			pVerifiedCmd->m_CRC = g_PacketManager->GetModifableCommand( )->GetChecksum( );
		}

		return;
	}

	g_RageBot->FakeDuck( );
	g_RageBot->SaveMovementData( );
	g_RageBot->PredictAutoStop( );
	
	g_PredictionSystem->Instance( );
	g_LocalAnimations->SetupShootPosition( );
	
	g_Movement->EdgeJump( );
	g_RageBot->UpdatePeekState( );
	g_GrenadePrediction->OnCreateMove( g_PacketManager->GetModifableCommand( ) );

	g_FakeLag->Instance( );
	g_RageBot->Instance( );
	g_ExploitSystem->BreakLagCompensation( );
	g_RageBot->ForceMovementData( );
	g_ExploitSystem->Instance( );
	g_AntiAim->Instance( );

	return g_PacketManager->FinishPacket( iSequence );
}

static void WriteUsercmd( bf_write* pBuffer, C_UserCmd* pToCmd, C_UserCmd* pFromCmd )
{
	auto WriteCmd = g_Globals.m_AddressList.m_WriteUsercmd;
	__asm
	{
		mov     ecx, pBuffer
		mov     edx, pToCmd
		push    pFromCmd
		call    WriteCmd
		add     esp, 4
	}
}

bool __fastcall C_Hooks::hkWriteUsercmdDeltaToBuffer( LPVOID pEcx, uint32_t, int32_t iSlot, bf_write* pBuffer, int32_t iFrom, int32_t iTo, bool bNewCmd )
{
	if ( g_ExploitSystem->GetForcedShiftAmount( ) < 1 || g_ExploitSystem->GetShiftMode( ) != MODE::SHIFT_BUFFER )
		return g_Globals.m_Hooks.m_Originals.m_WriteUsercmdDeltaToBuffer( pEcx, iSlot, pBuffer, iFrom, iTo, bNewCmd );

	int32_t iExtraCommands = g_ExploitSystem->GetForcedShiftAmount( );
	g_ExploitSystem->ResetShiftAmount( );

	int32_t* pNumBackupCommands = ( int32_t* )( ( uintptr_t )( pBuffer ) - 0x30 );
	int32_t* pNumNewCommands = ( int32_t* )( ( uintptr_t )( pBuffer ) - 0x2C );
	
	int32_t iNewCommands = *pNumNewCommands;
	int32_t iNextCommand = g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) + g_Globals.m_Interfaces.m_ClientState->m_nLastOutgoingCommand( ) + 1;

	*pNumBackupCommands = 0;

	for ( iTo = iNextCommand - iNewCommands + 1; iTo <= iNextCommand; iTo++ )
	{
		if ( !g_Globals.m_Hooks.m_Originals.m_WriteUsercmdDeltaToBuffer( pEcx, iSlot, pBuffer, iFrom, iTo, true ) )
			return false;

		iFrom = iTo;
	}

	*pNumNewCommands = iNewCommands + iExtraCommands;

	C_UserCmd* pCmd = g_Globals.m_Interfaces.m_Input->GetUserCmd( iSlot, iFrom );
	if ( !pCmd )
		return true;

	C_UserCmd ToCmd = *pCmd;
	C_UserCmd FromCmd = *pCmd;

	ToCmd.m_nCommand++;
	ToCmd.m_nTickCount += 1338;

	for ( int32_t i = iNewCommands; i <= iNewCommands + iExtraCommands; i++ )
	{
		WriteUsercmd( pBuffer, &ToCmd, &FromCmd );

		ToCmd.m_nTickCount--;
		ToCmd.m_nCommand--;
		
		FromCmd.m_nTickCount = ToCmd.m_nTickCount + 1;
		FromCmd.m_nCommand = ToCmd.m_nCommand + 1;
	}

	return true;
}

__declspec( naked ) void __stdcall C_Hooks::hkCreateMove_Proxy( int32_t iSequence, float_t flFrameTime, bool bIsActive )
{
	__asm
	{
		push ebx
		push esp
		push dword ptr[ esp + 20 ]
		push dword ptr[ esp + 0Ch + 8 ]
		push dword ptr[ esp + 10h + 4 ]
		call hkCreateMove
		pop ebx
		retn 0Ch
	}
}