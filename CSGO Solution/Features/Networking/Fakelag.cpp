#include "Fakelag.hpp"
#include "Networking.hpp"
#include "../Packet/PacketManager.hpp"
#include "../Settings.hpp"

#include "../Animations/LocalAnimations.hpp"
#include "../Animations/LagCompensation.hpp"
#include "../RageBot/RageBot.hpp"
#include "../Exploits/Exploits.hpp"

void C_FakeLag::Instance( )
{
	if ( g_Globals.m_Packet.m_bFakeDuck )
		return;

	int32_t iChokeAmount = -1;
	if ( g_Settings->m_bAntiAim )
		iChokeAmount = 1;

	if ( g_Settings->m_bFakeLagEnabled )
	{
		iChokeAmount = g_Settings->m_iLagLimit;
		if ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND )
		{
			if ( g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) > 23.40f )
				if ( g_Settings->m_aFakelagTriggers[ FAKELAG_MOVE ] )
					iChokeAmount = g_Settings->m_iTriggerLimit;
		}
		else if ( g_Settings->m_aFakelagTriggers[ FAKELAG_AIR ] )
			iChokeAmount = g_Settings->m_iTriggerLimit;

		if ( m_iForcedChoke )
			iChokeAmount = m_iForcedChoke;
	}

	if ( g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) < iChokeAmount )
		g_PacketManager->GetModifablePacket( ) = false;

	if ( g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) > 13 || ( ( *g_Globals.m_Interfaces.m_GameRules )->IsValveDS( ) && g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) > 5 ) ) 
		g_PacketManager->GetModifablePacket( ) = true;

	if ( ( *g_Globals.m_Interfaces.m_GameRules )->IsFreezePeriod( ) )
		g_PacketManager->GetModifablePacket( ) = true;

	C_BaseCombatWeapon* pCombatWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( pCombatWeapon )
	{
		if ( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_ATTACK )
		{
			bool bIsRevolver = false;
			if ( pCombatWeapon )
				if ( pCombatWeapon->m_iItemDefinitionIndex( ) == WEAPON_REVOLVER )
					bIsRevolver = true;

			int32_t nShiftAmount = g_ExploitSystem->GetShiftAmount( );
			if ( nShiftAmount > 9 || bIsRevolver )
				nShiftAmount = 0;

			bool bCanFire = g_Globals.m_LocalPlayer->CanFire( nShiftAmount, bIsRevolver );
			if ( bCanFire )
				g_PacketManager->GetModifablePacket( ) = true;
		}
	}

	if ( g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) > g_Globals.m_Packet.m_MaxChoke )
		g_PacketManager->GetModifablePacket( ) = true;
	
	if ( g_PacketManager->GetModifablePacket( ) )
		m_iForcedChoke = 0;

	if ( g_Settings->m_bFakeLagEnabled )
	{
		if ( g_Settings->m_aFakelagTriggers[ FAKELAG_PEEK ] )
		{
			if ( g_Globals.m_Peek.m_bIsPeeking )
			{
				if ( !m_iForcedChoke )
					g_PacketManager->GetModifablePacket( ) = true;

				m_iForcedChoke = g_Settings->m_iTriggerLimit;
			}
		}
	}

	if ( m_bDidForceTicksAllowed )
		return;

	return this->ForceTicksAllowedForProcessing( );
}

void C_FakeLag::ForceTicksAllowedForProcessing( )
{
	g_PacketManager->GetModifablePacket( ) = false;
	if ( g_Globals.m_Interfaces.m_ClientState->m_nChokedCommands( ) < 14 )
		return;

	m_bDidForceTicksAllowed = true;
}

void C_FakeLag::ResetData( )
{
	m_bDidForceTicksAllowed = false;
	m_iForcedChoke = NULL;
}