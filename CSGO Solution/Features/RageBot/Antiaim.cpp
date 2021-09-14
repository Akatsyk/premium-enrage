#include "Antiaim.hpp"
#include "../Packet/PacketManager.hpp"
#include "../Settings.hpp"
#include "../Tools/Tools.hpp"
#include "../SDK/Math/Math.hpp"
#include "../Exploits/Exploits.hpp"
#include "../Networking/Networking.hpp"

void C_AntiAim::Instance( )
{
	int32_t nCommand = g_PacketManager->GetModifableCommand( )->m_nCommand;
	if ( !g_Settings->m_bAntiAim )
	{
		if ( g_PacketManager->GetModifablePacket( ) )
			g_PacketManager->SetFakeAngles( g_PacketManager->GetModifableCommand( )->m_angViewAngles );

		return;
	}

	m_AntiAimConditions[ nCommand % MULTIPLAYER_BACKUP ] = this->GetAntiAimConditions( );
	if ( !m_AntiAimConditions[ nCommand % MULTIPLAYER_BACKUP ] )
	{
		if ( !g_Globals.m_Packet.m_bFakeDuck )
		{
			g_PacketManager->GetModifablePacket( ) = true;
			g_PacketManager->SetFakeAngles( g_PacketManager->GetModifableCommand( )->m_angViewAngles );
		}

		return;
	}
	
	float_t flFinalPitch = 0.0f;
	switch ( g_Settings->m_iPitchMode )
	{
		case 1: flFinalPitch = 89.0f; break;
		case 2: flFinalPitch = -89.0f; break;
		case 3: flFinalPitch = -540.0f; break;
		case 4: flFinalPitch = 540.0f; break;
		default: break;
	}

	if ( flFinalPitch )
		if ( m_AntiAimConditions[ nCommand % MULTIPLAYER_BACKUP ] == ANTIAIM_FULL )
			g_PacketManager->GetModifableCommand( )->m_angViewAngles.pitch = flFinalPitch;
	
	float_t flFinalYaw = g_PacketManager->GetModifableCommand( )->m_angViewAngles.yaw;
	if ( m_AntiAimConditions[ nCommand % MULTIPLAYER_BACKUP ] )
	{
		if ( m_AntiAimConditions[ nCommand % MULTIPLAYER_BACKUP ] == ANTIAIM_FULL )
		{
			if ( g_Settings->m_bAntiAimAtTargets )
				flFinalYaw = this->GetTargetYaw( );
			else if ( m_AntiAimConditions[ nCommand % MULTIPLAYER_BACKUP ] == ANTIAIM_FULL )
				flFinalYaw += 180.0f;
		}

		if ( g_PacketManager->GetModifablePacket( ) )
		{
			if ( g_Tools->IsBindActive( g_Settings->m_aInverter ) )
				m_iDesyncSide = 1;
			else
				m_iDesyncSide = -1;

			m_bShouldUseAlternativeSide = false;
			if ( g_Settings->m_aInverterConditions[ 0 ] && ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) && g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) < 23.40f && g_Globals.m_LocalPlayer->m_vecVelocity( ).z == 0.0f )
			{
				m_iFinalSide = -m_iFinalSide;
				m_bShouldUseAlternativeSide = true;
			}
			else if ( g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) > 23.40f && g_Globals.m_LocalPlayer->m_vecVelocity( ).z == 0.0f && ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
			{
				if ( g_Settings->m_aInverterConditions[ 1 ] )
				{
					m_iFinalSide = -m_iFinalSide;
					m_bShouldUseAlternativeSide = true;
				}
			}
			else if ( !( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
			{
				if ( g_Settings->m_aInverterConditions[ 2 ] )
				{
					m_iFinalSide = -m_iFinalSide;
					m_bShouldUseAlternativeSide = true;
				}
			}

			if ( !m_bShouldUseAlternativeSide )
				m_iFinalSide = m_iDesyncSide;

			if ( m_AntiAimConditions[ nCommand % MULTIPLAYER_BACKUP ] == ANTIAIM_FULL )
			{
				if ( m_bSwitch )
					flFinalYaw += g_Settings->m_iJitterAmount;
				else
					flFinalYaw -= g_Settings->m_iJitterAmount;
			}

			m_bSwitch = !m_bSwitch;
		}
	
		g_PacketManager->GetModifableCommand( )->m_angViewAngles.yaw = Math::NormalizeAngle( flFinalYaw + g_Settings->m_iYawAddOffset );
	}

	int32_t iDesyncSide = m_iDesyncSide;
	if ( m_AntiAimConditions[ nCommand % MULTIPLAYER_BACKUP ] == ANTIAIM_DESYNC )
		iDesyncSide = -iDesyncSide;

	if ( m_bShouldUseAlternativeSide )
		iDesyncSide = m_iFinalSide;

	g_PacketManager->SetFakeAngles( g_PacketManager->GetModifableCommand( )->m_angViewAngles );
	if ( m_AntiAimConditions[ nCommand % MULTIPLAYER_BACKUP ] == ANTIAIM_DESYNC )
		if ( m_AntiAimConditions[ ( nCommand - 1 ) % MULTIPLAYER_BACKUP ] == ANTIAIM_FULL )
			return;
	
	if ( !g_PacketManager->GetModifablePacket( ) )
	{
		float_t flDesyncAmount = 0.0f;
		if ( m_iDesyncSide > 0 )
			flDesyncAmount = g_Settings->m_iRightFakeLimit * 2.0f;
		else
			flDesyncAmount = g_Settings->m_iLeftFakeLimit * 2.0f;

		g_PacketManager->GetModifableCommand( )->m_angViewAngles.yaw += flDesyncAmount * iDesyncSide;
	}
}

float C_AntiAim::GetTargetYaw( )
{
	float_t flBestDistance = FLT_MAX;
	
	C_BasePlayer* pFinalPlayer = nullptr;
	for ( int32_t i = 1; i < 65; i++ )
	{
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( i );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) || pPlayer->IsDormant( ) )
			continue;

		if ( pPlayer->m_fFlags( ) & FL_FROZEN )
			continue;

		float_t flDistanceToPlayer = g_Globals.m_LocalPlayer->m_vecOrigin( ).DistTo( pPlayer->m_vecOrigin( ) );
		if ( flDistanceToPlayer > flBestDistance )
			continue;

		if ( flDistanceToPlayer > 1250.0f )
			continue;

		flBestDistance = flDistanceToPlayer;
		pFinalPlayer = pPlayer;
	}

	if ( !pFinalPlayer )
		return g_PacketManager->GetModifableCommand( )->m_angViewAngles.yaw + 180.0f;

	return Math::CalcAngle( g_Globals.m_LocalPlayer->GetAbsOrigin( ) + g_Globals.m_LocalPlayer->m_vecViewOffset( ), pFinalPlayer->GetAbsOrigin( ) ).yaw + 180.0f;
}

void C_AntiAim::JitterMove( )
{
	const float_t flSpeedPerTick = 5.0f / g_Networking->GetTickRate( );
	if ( !g_Settings->m_bJitterMove || ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_FROZEN ) || ( g_Globals.m_LocalPlayer->m_bGunGameImmunity( ) ) || ( *( g_Globals.m_Interfaces.m_GameRules ) )->IsFreezePeriod( ) )
		return;

	if ( g_Tools->IsBindActive( g_Settings->m_aSlowwalk ) )
		return;
    
	const float_t flTargetVelocity = ( ( g_PacketManager->GetModifableCommand( )->m_nCommand % g_Networking->GetTickRate( ) ) * flSpeedPerTick ) + 95.0f;
    if ( flTargetVelocity <= 100.0 ) 
		if ( flTargetVelocity >= 95.0 )
			m_flAccelerationSpeed = flTargetVelocity;
	else
		m_flAccelerationSpeed = 100.0;

    const float_t flWishVelocity = ( m_flAccelerationSpeed / 100.0 ) * g_Globals.m_LocalPlayer->GetMaxPlayerSpeed( );
    if ( flWishVelocity <= 0.0f )
		return;

    const float_t flMoveLength = Vector( g_PacketManager->GetModifableCommand( )->m_flForwardMove, g_PacketManager->GetModifableCommand( )->m_flSideMove, g_PacketManager->GetModifableCommand( )->m_flUpMove ).Length( );
    if ( flMoveLength < 10.0 || flMoveLength < flWishVelocity )
		return;

	g_PacketManager->GetModifableCommand( )->m_flForwardMove = ( g_PacketManager->GetModifableCommand( )->m_flForwardMove / flMoveLength ) * flWishVelocity;
    g_PacketManager->GetModifableCommand( )->m_flSideMove = ( g_PacketManager->GetModifableCommand( )->m_flSideMove / flMoveLength ) * flWishVelocity;
	g_PacketManager->GetModifableCommand( )->m_flUpMove = ( g_PacketManager->GetModifableCommand( )->m_flUpMove / flMoveLength ) * flWishVelocity;
}

void C_AntiAim::SlowWalk( bool bForcedWalk )
{
	bool bCanSlowwalk = true;
	if ( !g_Tools->IsBindActive( g_Settings->m_aSlowwalk ) )
		bCanSlowwalk = false;

	if ( ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_FROZEN ) || ( g_Globals.m_LocalPlayer->m_bGunGameImmunity( ) ) || ( *( g_Globals.m_Interfaces.m_GameRules ) )->IsFreezePeriod( ) )
		bCanSlowwalk = false;

	float_t flMovementLength = g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( );
	if ( ( !bCanSlowwalk && !bForcedWalk ) || !( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
		return;

	float_t flMaxSpeed = g_Globals.m_LocalPlayer->GetMaxPlayerSpeed( );
	if ( m_bWalkSwitch )
		flMaxSpeed *= 0.033999f;
	else
		flMaxSpeed *= 0.022999f;

	m_bWalkSwitch = !m_bWalkSwitch;
	if ( g_PacketManager->GetModifablePacket( ) )
	{
		if ( g_Settings->m_bJitterMove )
		{
			if ( m_bMoveSwitch )
				g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_SPEED;
			else
				g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_SPEED;
		}

		m_bMoveSwitch = !m_bMoveSwitch;
	}

	float_t flDistanceToMinimalSpeed = flMovementLength / flMaxSpeed;
	if ( flDistanceToMinimalSpeed <= 0.0f )
		return;

	g_PacketManager->GetModifableCommand( )->m_flForwardMove /= flDistanceToMinimalSpeed;
	g_PacketManager->GetModifableCommand( )->m_flSideMove /= flDistanceToMinimalSpeed;
}

void C_AntiAim::LegMovement( )
{
	if ( !g_Settings->m_bAntiAim || !( ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) ) )
		return;
	
	int32_t Conditions = this->GetAntiAimConditions( );
	if ( Conditions != ANTIAIM_FULL )
		return;

	g_PacketManager->GetModifableCommand( )->m_nButtons &= ~( IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK );
	if ( !g_Settings->m_iLegMovement )
	{
		if ( g_PacketManager->GetModifableCommand( )->m_flForwardMove > 0.0f )
		{
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_FORWARD;
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_BACK;
		}
		else if ( g_PacketManager->GetModifableCommand( )->m_flForwardMove < 0.0f )
		{
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_FORWARD;
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_BACK;
		}

		if ( g_PacketManager->GetModifableCommand( )->m_flSideMove > 0.0f )
		{
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_MOVERIGHT;
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_MOVELEFT;
		}
		else if ( g_PacketManager->GetModifableCommand( )->m_flSideMove < 0.0f )
		{
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_MOVERIGHT;
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_MOVELEFT;
		}
	}
	else
	{
		if ( g_PacketManager->GetModifableCommand( )->m_flForwardMove > 0.0f )
		{
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_FORWARD;
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_BACK;
		}
		else if ( g_PacketManager->GetModifableCommand( )->m_flForwardMove < 0.0f )
		{
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_FORWARD;
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_BACK;
		}

		if ( g_PacketManager->GetModifableCommand( )->m_flSideMove > 0.0f )
		{
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_MOVERIGHT;
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_MOVELEFT;
		}
		else if ( g_PacketManager->GetModifableCommand( )->m_flSideMove < 0.0f )
		{
			g_PacketManager->GetModifableCommand( )->m_nButtons &= ~IN_MOVERIGHT;
			g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_MOVELEFT;
		}
	}
}

void C_AntiAim::Micromovement( ) 
{
	if ( !g_Settings->m_bAntiAim || g_PacketManager->GetModifableCommand( )->m_nButtons & ( IN_MOVELEFT | IN_MOVERIGHT | IN_BACK | IN_FORWARD ) )
		return;

	float_t flVelocityLength = g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( );
	if ( flVelocityLength > 5.0f )
		return;
	
	float_t flSpeed = 1.1f;
	if ( ( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_DUCK ) || g_Globals.m_Packet.m_bFakeDuck )
		flSpeed *= 3.3333334f;

	if ( g_PacketManager->GetModifableCommand( )->m_nCommand & 1 )
		flSpeed = -flSpeed;

	g_PacketManager->GetModifableCommand( )->m_flForwardMove = flSpeed;
}

int32_t C_AntiAim::GetAntiAimConditions( )
{
	if ( !g_Settings->m_bAntiAim )
		return 0;

	C_BaseCombatWeapon* pCombatWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
	if ( !pCombatWeapon )
		return 0;

	if ( ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_FROZEN ) || ( *g_Globals.m_Interfaces.m_GameRules )->IsFreezePeriod( ) )
		return 0;

	if ( g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_LADDER )
	{
		if ( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_BACK )
			return 0;

		if ( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_FORWARD )
			return 0;
	}

	if ( g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_NOCLIP )
		return 0;

	if ( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_ATTACK )
	{
		bool bIsRevolver = false;
		if ( pCombatWeapon )
			if ( pCombatWeapon->m_iItemDefinitionIndex( ) == WEAPON_REVOLVER )
				bIsRevolver = true;

		int32_t nShiftAmount = g_ExploitSystem->GetShiftAmount( );
		if ( nShiftAmount > 9 || bIsRevolver )
			nShiftAmount = 0;

		if ( g_Globals.m_LocalPlayer->CanFire( nShiftAmount, bIsRevolver ) )
			return 0;
	}

	if ( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_ATTACK2 )
		if ( pCombatWeapon->IsKnife( ) )
			return 0;

	if ( pCombatWeapon->IsGrenade( ) && pCombatWeapon->m_fThrowTime( ) > 0.0f )
		return 0;

	if ( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_USE )
		return 2;

	return 1;
}