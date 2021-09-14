#include "PacketManager.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../Animations/LocalAnimations.hpp"
#include "../RageBot/Antiaim.hpp"
#include "../RageBot/RageBot.hpp"
#include "../Settings.hpp"
#include "../BuyBot/BuyBot.hpp"
#include "../SDK/Math/Math.hpp"
#include "../Features/Movement/AutoPeek.hpp"
#include "../Networking/Networking.hpp"

bool C_PacketManager::SetupPacket( int32_t iSequence, bool* pbSendPacket )
{
	this->m_pUserCmd = g_Globals.m_Interfaces.m_Input->GetUserCmd( iSequence );
	if ( !this->m_pUserCmd || !this->m_pUserCmd->m_nCommand )
		return false;

	if ( !g_Globals.m_LocalPlayer->IsAlive( ) )
		return false;

	this->m_pbSendPacket = pbSendPacket;
	return true;
}

void C_PacketManager::FinishPacket( int32_t iSequence )
{
	C_VerifiedUserCmd* pVerifiedCommand = g_Globals.m_Interfaces.m_Input->GetVerifiedCmd( iSequence );
	if ( !pVerifiedCommand )
		return g_PredictionSystem->ResetPacket( );

	g_Globals.m_Packet.m_bInCreateMove = false;
	if ( g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) > 13 )
		*m_pbSendPacket = true;

	g_BuyBot->OnCreateMove( );
	if ( g_Settings->m_bAntiUntrusted || ( *g_Globals.m_Interfaces.m_GameRules )->IsValveDS( ) )
	{
		Math::Normalize3( m_pUserCmd->m_angViewAngles );
		Math::ClampAngles( m_pUserCmd->m_angViewAngles );
	}

	// autopeek
	g_AutoPeek->Instance( );
	g_RageBot->RestorePlayers( );

	Math::FixMovement( m_pUserCmd );

	pVerifiedCommand->m_Cmd = *m_pUserCmd;
	pVerifiedCommand->m_CRC = m_pUserCmd->GetChecksum( );

	// process the net
	if ( *m_pbSendPacket )
		m_aCommandList.emplace_back( m_pUserCmd->m_nCommand );
	else
		this->FinishNetwork( );

	// анимируем локал плеера
	g_LocalAnimations->Instance( );
	
	// резетаем данные из предикшна
	return g_PredictionSystem->ResetPacket( );
}

void C_PacketManager::FinishNetwork( )
{
	int32_t iChokedCommand = g_Globals.m_Interfaces.m_ClientState->m_pNetChannel( )->m_iChokedCommands;
	int32_t iSequenceNumber = g_Globals.m_Interfaces.m_ClientState->m_pNetChannel( )->m_iOutSequenceNr;

	g_Globals.m_Interfaces.m_ClientState->m_pNetChannel( )->m_iChokedCommands = 0;
		
	g_Globals.m_Interfaces.m_ClientState->m_pNetChannel( )->SendDatagram( NULL );

	g_Globals.m_Interfaces.m_ClientState->m_pNetChannel( )->m_iOutSequenceNr = iSequenceNumber;
	g_Globals.m_Interfaces.m_ClientState->m_pNetChannel( )->m_iChokedCommands = iChokedCommand;
}

bool C_PacketManager::ShouldProcessPacketStart( int32_t iCommand )
{
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
		return true;

	for ( auto m_Cmd = m_aCommandList.begin( ); m_Cmd != m_aCommandList.end( ); m_Cmd++ )
	{
		if ( *m_Cmd != iCommand )
			continue;
	
		m_aCommandList.erase( m_Cmd );
		return true;
	}

	return false;
}

C_UserCmd* C_PacketManager::GetModifableCommand( )
{
	return this->m_pUserCmd;
}

bool& C_PacketManager::GetModifablePacket( )
{
	return *this->m_pbSendPacket;
}