#include "Thirdperson.hpp"
#include "../Settings.hpp"
#include "../Tools/Tools.hpp"
#include "../SDK/Math/Math.hpp"

void TraceHull( const Vector& vec_start, const Vector& vec_end, const unsigned int n_mask, const Vector& extents, trace_t* p_trace )
{
	CTraceFilterWorldAndPropsOnly Filter;

	Ray_t ray;
	ray.Init( vec_start, vec_end );
	ray.m_Extents = extents;
	ray.m_IsRay = false;

	g_Globals.m_Interfaces.m_EngineTrace->TraceRay( ray, n_mask, &Filter, p_trace );
}

void C_ThirdPerson::Instance( )
{
	if ( !g_Globals.m_LocalPlayer )
		return;

	if ( !g_Globals.m_LocalPlayer->IsAlive( ) )
	{
		g_Globals.m_Interfaces.m_Input->m_bCameraInThirdPerson = false;
		m_flThirdpersonDistance = 50.0f;

		g_Globals.m_LocalPlayer->m_iObserverMode( ) = ObserverType_t::OBS_MODE_CHASE;
		return;
	}

	bool bIsEnabled = g_Tools->IsBindActive( g_Settings->m_aThirdPerson );
	if ( bIsEnabled )
		m_flThirdpersonDistance = min( max( m_flThirdpersonDistance, 30.0f ) + 6.0f, g_Settings->m_iThirdPersonDistance );
	else
		m_flThirdpersonDistance = max( m_flThirdpersonDistance - 6.0f, 30.0f );

	if ( m_flThirdpersonDistance <= ( bIsEnabled ? 50.0f : 30.0f ) )
	{
		g_Globals.m_Interfaces.m_Input->m_bCameraInThirdPerson = false;
		return;
	}

	Vector vecEyePos = g_Globals.m_LocalPlayer->GetAbsOrigin( ) + g_Globals.m_LocalPlayer->m_vecViewOffset( );
	if ( g_Globals.m_Packet.m_bVisualFakeDuck )
		vecEyePos = g_Globals.m_LocalPlayer->GetAbsOrigin( ) + g_Globals.m_Interfaces.m_GameMovement->GetPlayerViewOffset( false );

	QAngle angViewAngles;
	g_Globals.m_Interfaces.m_EngineClient->GetViewAngles( &angViewAngles );

	g_Globals.m_Interfaces.m_Input->m_bCameraInThirdPerson = true;
	g_Globals.m_Interfaces.m_Input->m_vecCameraOffset = Vector( angViewAngles.pitch, angViewAngles.yaw, m_flThirdpersonDistance );

	CGameTrace Trace;

	float flExtent = 12.f + ( g_Settings->m_iCameraDistance / 4.8f - 18.f );

	Vector vecForward = Vector( 0, 0, 0 );
	Math::AngleVectors( QAngle( angViewAngles.pitch, angViewAngles.yaw, 0.0f ), vecForward );
	TraceHull( vecEyePos, vecEyePos - vecForward * g_Globals.m_Interfaces.m_Input->m_vecCameraOffset.z, MASK_SOLID & ~CONTENTS_MONSTER, Vector( flExtent, flExtent, flExtent ), &Trace );
	g_Globals.m_Interfaces.m_Input->m_vecCameraOffset.z *= Trace.fraction;

	if ( g_Globals.m_Interfaces.m_Input->m_vecCameraOffset.z < min( 30.0f, g_Settings->m_iThirdPersonDistance ) )
		g_Globals.m_Interfaces.m_Input->m_bCameraInThirdPerson = false;
}