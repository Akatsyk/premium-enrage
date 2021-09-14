#include "AutoPeek.hpp"
#include "../Tools/Tools.hpp"
#include "../Render.hpp"
#include "../SDK/Math/Math.hpp"
#include "../Packet/PacketManager.hpp"

void C_AutoPeek::Instance( )
{
	if ( m_bRetreated )
		m_vecStartPosition.Zero( );
	else if ( g_Tools->IsBindActive( g_Settings->m_aAutoPeek ) )
	{
		if ( !m_bTurnedOn )
		{
			m_bTurnedOn = true;
			m_bWaitAnimationProgress = true;
			m_bNegativeSide = false;
			
			m_flAnimationTime = g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime + .1f;

			m_vecStartPosition = g_Globals.m_LocalPlayer->GetAbsOrigin( );
		}
	}
	else if ( m_bTurnedOn )
	{
		m_bTurnedOn = false;
		m_bWaitAnimationProgress = true;
		m_bNegativeSide = true;

		m_flAnimationTime = g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime + .1f;
	}

	if ( !m_bRetreat )
		if ( g_PacketManager->GetModifableCommand( )->m_nButtons & IN_ATTACK )
			m_bRetreat = true;

	if ( !m_bRetreat || !m_bTurnedOn )
		return;

	auto vecDifference = g_Globals.m_LocalPlayer->GetAbsOrigin( ) - m_vecStartPosition;
	if ( vecDifference.Length2D( ) <= 5.0f )
	{
		m_bRetreat = false;
		return;
	}
	
	QAngle angWishAngles;
	g_Globals.m_Interfaces.m_EngineClient->GetViewAngles( &angWishAngles );

	float_t flVelocityX = vecDifference.x * cos( angWishAngles.yaw / 180.0f * M_PI) + vecDifference.y * sin( angWishAngles.yaw / 180.0f * M_PI );
	float_t flVelocityY = vecDifference.y * cos( angWishAngles.yaw / 180.0f * M_PI) - vecDifference.x * sin( angWishAngles.yaw / 180.0f * M_PI );

	g_PacketManager->GetModifableCommand( )->m_flForwardMove = -flVelocityX * 20.0f;
	g_PacketManager->GetModifableCommand( )->m_flSideMove = flVelocityY * 20.0f;
}

void C_AutoPeek::DrawCircle( )
{
	float_t flAnimationProgress = 1.0f;
	if ( m_bWaitAnimationProgress )
	{
		flAnimationProgress = 1.0f - ( ( m_flAnimationTime - g_Globals.m_Interfaces.m_GlobalVars->m_flRealTime ) / .1f );
		if ( m_bNegativeSide )
			flAnimationProgress = 1.0f - flAnimationProgress;

		if ( flAnimationProgress == 0.0f || flAnimationProgress == 1.0f )
			m_bWaitAnimationProgress = false;
	}

	flAnimationProgress = std::clamp( flAnimationProgress, 0.0f, 1.0f );
	if ( !m_bTurnedOn )
	{
		if ( m_bWaitAnimationProgress )
			g_Render->RenderCircle3D( m_vecStartPosition, 32, 30.0f * flAnimationProgress, Color( 255, 255, 255, 255 ) );
		else
			m_vecStartPosition = Vector( 0, 0, 0 );

		return;
	}

	return g_Render->RenderCircle3D( m_vecStartPosition, 32, 30.0f * flAnimationProgress, Color( 255, 255, 255, 255 ) );
}