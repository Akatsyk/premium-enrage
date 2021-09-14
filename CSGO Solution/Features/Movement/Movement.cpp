#include "Movement.hpp"
#include "../Packet/PacketManager.hpp"
#include "../Prediction/EnginePrediction.hpp"
#include "../SDK/Math/Math.hpp"

void C_Movement::BunnyHop( )
{
	C_UserCmd* pCmd = g_PacketManager->GetModifableCommand( );
	if ( !g_Globals.m_LocalPlayer->IsAlive( ) || g_Globals.m_LocalPlayer->m_nRenderMode( ) == 9 || g_Globals.m_LocalPlayer->m_nRenderMode( ) == 8 )
		 return;

	if ( !g_Settings->m_bBunnyHop )
		return;

	if ( g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_NOCLIP || 
		 g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_LADDER )
		 return;

	if ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND )
		return;
	
	pCmd->m_nButtons &= ~IN_JUMP;
}

void C_Movement::AutoStrafe( )
{
	C_UserCmd* pCmd = g_PacketManager->GetModifableCommand( );
	if ( !g_Globals.m_LocalPlayer->IsAlive( ) || g_Globals.m_LocalPlayer->m_nRenderMode( ) == 9 || g_Globals.m_LocalPlayer->m_nRenderMode( ) == 8 )
		 return;

	if ( !g_Settings->m_bAutoStrafe )
		return;

	if ( g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_NOCLIP || 
		 g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_LADDER )
		 return;

	if ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND )
		return;

	if ( g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) <= 5.0f )
	{
		C_BaseCombatWeapon* pCombatWeapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
		if ( pCombatWeapon )
		{
			if ( pCombatWeapon->m_iItemDefinitionIndex( ) != WEAPON_SSG08 )
			{		
				if ( !g_Settings->m_bSpeedBoost )
					return;
			}
			else
				return;
		}
	}

	if ( !g_Settings->m_bWASDStrafe )
	{
		pCmd->m_flForwardMove = ( 10000.f / g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) > 450.f) ? 450.f : 10000.f / g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( );
		pCmd->m_flSideMove = ( pCmd->m_nMouseDirectionX != 0 ) ? ( pCmd->m_nMouseDirectionX < 0.0f ) ? -450.f : 450.f : ( pCmd->m_nCommand & 1 ) ? -450.f : 450.f;	

		return;
	}

	static bool bFlip = true;
	static float flOldYaw = pCmd->m_angViewAngles.yaw;

	Vector vecVelocity		= g_Globals.m_LocalPlayer->m_vecVelocity( );
	vecVelocity.z			= 0.0f;

	float_t flForwardMove	= pCmd->m_flForwardMove;
	float_t flSideMove		= pCmd->m_flSideMove;

	float flTurnVelocityModifier = bFlip ? 1.5f : -1.5f;
	QAngle angViewAngles = pCmd->m_angViewAngles;

	if ( flForwardMove || flSideMove )
	{
		pCmd->m_flForwardMove = 0.0f;
		pCmd->m_flSideMove = 0.0f;

		float m_flTurnAngle = atan2(-flSideMove, flForwardMove);
		angViewAngles.yaw += m_flTurnAngle * M_RADPI;
	}
	else if ( flForwardMove )
		pCmd->m_flForwardMove = 0.0f;

	float flStrafeAngle = RAD2DEG( atan( 15.0f / vecVelocity.Length2D( ) ) );
	if ( flStrafeAngle > 90.0f )
		flStrafeAngle = 90.0f;
	else if ( flStrafeAngle < 0.0f )
		flStrafeAngle = 0.0f;

	Vector vecTemp = Vector( 0.0f, angViewAngles.yaw - flOldYaw, 0.0f );
	vecTemp.y = Math::NormalizeAngle( vecTemp.y );
	flOldYaw = angViewAngles.yaw;
	
	float flYawDelta = vecTemp.y;
	float flAbsYawDelta = fabs( flYawDelta );
	if ( flAbsYawDelta <= flStrafeAngle || flAbsYawDelta >= 30.0f )
	{
		QAngle angVelocityAngle;
		Math::VectorAngles( vecVelocity, angVelocityAngle );

		vecTemp		= Vector( 0.0f, angViewAngles.yaw - angVelocityAngle.yaw, 0.0f );
		vecTemp.y	= Math::NormalizeAngle( vecTemp.y );

		float flVelocityAngleYawDelta	= vecTemp.y;
		float flVelocityDegree			= std::clamp( RAD2DEG( atan( 30.0f / vecVelocity.Length2D( ) ) ), 0.0f, 90.0f ) * 0.01f;
		 
		if ( flVelocityAngleYawDelta <= flVelocityDegree || vecVelocity.Length2D( ) <= 15.0f )
		{
			if ( -flVelocityDegree <= flVelocityAngleYawDelta || vecVelocity.Length2D( ) <= 15.0f )
			{
				angViewAngles.yaw += flStrafeAngle * flTurnVelocityModifier;
				pCmd->m_flSideMove = 450.0f * flTurnVelocityModifier;
			}
			else
			{
				angViewAngles.yaw = angVelocityAngle.yaw - flVelocityDegree;
				pCmd->m_flSideMove = 450.0f;
			}
		}
		else
		{
			angViewAngles.yaw = angVelocityAngle.yaw + flVelocityDegree;
			pCmd->m_flSideMove = -450.0f;
		}
	}
	else if ( flYawDelta > 0.0f )
		pCmd->m_flSideMove = 450.0f;
	else if ( flYawDelta < 0.0f )
		pCmd->m_flSideMove = 450.0f;

	Vector vecMove = Vector( pCmd->m_flForwardMove, pCmd->m_flSideMove, 0.0f );
	float flSpeed = vecMove.Length( );

	QAngle angMoveAngle;
	Math::VectorAngles( vecMove, angMoveAngle );

	float flNormalizedX = fmod( pCmd->m_angViewAngles.pitch + 180.0f, 360.0f ) - 180.0f;
	float flNormalizedY = fmod( pCmd->m_angViewAngles.yaw + 180.0f, 360.0f ) - 180.0f;
	float flYaw = DEG2RAD( ( flNormalizedY - angViewAngles.yaw ) + angMoveAngle.yaw );

	if ( pCmd->m_angViewAngles.pitch <= 200.0f && ( flNormalizedX >= 90.0f || flNormalizedX <= -90.0f || ( pCmd->m_angViewAngles.pitch >= 90.0f && pCmd->m_angViewAngles.pitch <= 200.0f) || pCmd->m_angViewAngles.pitch <= -90.0f ) )
		pCmd->m_flForwardMove = -cos( flYaw ) * flSpeed;
	else
		pCmd->m_flForwardMove = cos( flYaw ) * flSpeed;

	pCmd->m_flSideMove = sin( flYaw ) * flSpeed;

	bFlip = !bFlip;
}

void C_Movement::MouseCorrection( )
{
	QAngle angOldViewAngles;
	g_Globals.m_Interfaces.m_EngineClient->GetViewAngles( &angOldViewAngles );

	float delta_x = std::remainderf( g_PacketManager->GetModifableCommand( )->m_angViewAngles.pitch - angOldViewAngles.pitch, 360.0f );
	float delta_y = std::remainderf( g_PacketManager->GetModifableCommand( )->m_angViewAngles.yaw - angOldViewAngles.yaw, 360.0f );

	if ( delta_x != 0.0f ) 
	{
		float mouse_y = -( ( delta_x / g_Globals.m_ConVars.m_Pitch->GetFloat( ) ) / g_Globals.m_ConVars.m_Sensitivity->GetFloat( ) );
		short mousedy;
		if ( mouse_y <= 32767.0f ) {
			if ( mouse_y >= -32768.0f ) {
				if ( mouse_y >= 1.0f || mouse_y < 0.0f ) {
					if ( mouse_y <= -1.0f || mouse_y > 0.0f )
						mousedy = static_cast<short>( mouse_y );
					else
						mousedy = -1;
				}
				else {
					mousedy = 1;
				}
			}
			else {
				mousedy = 0x8000u;
			}
		}
		else {
			mousedy = 0x7FFF;
		}

		g_PacketManager->GetModifableCommand( )->m_nMouseDirectionY = mousedy;
	}

	if ( delta_y != 0.0f )
	{
		float mouse_x = -( ( delta_y / g_Globals.m_ConVars.m_Yaw->GetFloat( ) ) / g_Globals.m_ConVars.m_Sensitivity->GetFloat( ) );
		short mousedx;
		if ( mouse_x <= 32767.0f ) {
			if ( mouse_x >= -32768.0f ) {
				if ( mouse_x >= 1.0f || mouse_x < 0.0f ) {
					if ( mouse_x <= -1.0f || mouse_x > 0.0f )
						mousedx = static_cast<short>( mouse_x );
					else
						mousedx = -1;
				}
				else {
					mousedx = 1;
				}
			}
			else {
				mousedx = 0x8000u;
			}
		}
		else {
			mousedx = 0x7FFF;
		}

		g_PacketManager->GetModifableCommand( )->m_nMouseDirectionX = mousedx;
	}
}

void C_Movement::FastStop( )
{
	if ( !g_Globals.m_LocalPlayer->IsAlive( ) || g_Globals.m_LocalPlayer->m_nRenderMode( ) == 9 || g_Globals.m_LocalPlayer->m_nRenderMode( ) == 8 )
		 return;

	if ( !g_Settings->m_bFastStop )
		return;

	if ( g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_NOCLIP || 
		 g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_LADDER )
		 return;

	if ( g_PacketManager->GetModifableCommand( )->m_nButtons & ( IN_JUMP | IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK ) )
		return;

	if ( !( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND ) )
		return;

	if ( g_Globals.m_LocalPlayer->m_vecVelocity( ).Length2D( ) <= g_Globals.m_LocalPlayer->GetMaxPlayerSpeed( ) * 0.34f )
	{
		g_PacketManager->GetModifableCommand( )->m_flForwardMove = g_PacketManager->GetModifableCommand( )->m_flSideMove = 0.0f;
		return;
	}

	QAngle angResistance = QAngle( 0, 0, 0 );
    Math::VectorAngles( ( g_Globals.m_LocalPlayer->m_vecVelocity( ) * -1.f ), angResistance );

	angResistance.yaw = g_PacketManager->GetModifableCommand( )->m_angViewAngles.yaw - angResistance.yaw;
	angResistance.pitch = g_PacketManager->GetModifableCommand( )->m_angViewAngles.pitch - angResistance.pitch;

    Vector vecResistance = Vector( 0, 0, 0 );
    Math::AngleVectors( angResistance, vecResistance );

    g_PacketManager->GetModifableCommand( )->m_flForwardMove = std::clamp( vecResistance.x, -450.f, 450.0f );
	g_PacketManager->GetModifableCommand( )->m_flSideMove = std::clamp( vecResistance.y, -450.f, 450.0f );
}

void C_Movement::EdgeJump( )
{
	if ( !g_Globals.m_LocalPlayer->IsAlive( ) || g_Globals.m_LocalPlayer->m_nRenderMode( ) == 9 || g_Globals.m_LocalPlayer->m_nRenderMode( ) == 8 )
		 return;

	if ( !g_Settings->m_bEdgeJump )
		return;

	if ( g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_NOCLIP || 
		 g_Globals.m_LocalPlayer->GetMoveType( ) == MOVETYPE_LADDER )
		 return;

	if ( !( g_PredictionSystem->GetNetvars( g_PacketManager->GetModifableCommand( )->m_nCommand ).m_fFlags & FL_ONGROUND ) )
		return;

	if ( g_Globals.m_LocalPlayer->m_fFlags( ) & FL_ONGROUND )
		return;

	g_PacketManager->GetModifableCommand( )->m_nButtons |= IN_JUMP;
	
	g_PredictionSystem->RestoreNetvars( g_PacketManager->GetModifableCommand( )->m_nCommand );
	return g_PredictionSystem->Repredict( );
}