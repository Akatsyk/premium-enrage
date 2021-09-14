#include "../Hooks.hpp"
#include "../Menu.hpp"
#include "../Settings.hpp"
#include "../Features/Visuals/Players.hpp"
#include "../Features/Visuals/Thirdperson.hpp"
#include "../Features/Visuals/ShotChams.hpp"

int __fastcall C_Hooks::hkDoPostScreenEffects( LPVOID pEcx, uint32_t )
{
	if ( g_Globals.m_LocalPlayer )
	{
		g_ShotChams->OnDrawModel( );
		g_PlayerESP->RenderGlow( );
	}

	return g_Globals.m_Hooks.m_Originals.m_DoPostScreenEffects( pEcx );
}

void __fastcall C_Hooks::hkOverrideView( LPVOID pEcx, uint32_t, C_ViewSetup* pSetupView )
{
	if ( !g_Globals.m_LocalPlayer )
		return g_Globals.m_Hooks.m_Originals.m_OverrideView( pEcx, pSetupView );

	if ( !g_Globals.m_LocalPlayer->IsAlive( ) )
	{
		g_Globals.m_Interfaces.m_Input->m_bCameraInThirdPerson = false;
		return g_Globals.m_Hooks.m_Originals.m_OverrideView( pEcx, pSetupView );
	}

	if ( g_Globals.m_Packet.m_bVisualFakeDuck )
		pSetupView->vecOrigin = g_Globals.m_LocalPlayer->GetAbsOrigin( ) + g_Globals.m_Interfaces.m_GameMovement->GetPlayerViewOffset( false );

	if ( !g_Globals.m_LocalPlayer->m_bIsScoped( ) || g_Settings->m_bOverrideFOVWhileScoped )
		pSetupView->flFOV = g_Settings->m_iCameraDistance;

	if ( g_Globals.m_LocalPlayer->m_hViewModel( ).Get( ) )
	{
		QAngle angViewSetup = pSetupView->angView;
		if ( g_Settings->m_iViewmodelRoll )
			g_Globals.m_LocalPlayer->m_hViewModel( )->SetAbsoluteAngles( QAngle( angViewSetup.pitch, angViewSetup.yaw, g_Settings->m_iViewmodelRoll ) );
	}

	g_ThirdPerson->Instance( );
	return g_Globals.m_Hooks.m_Originals.m_OverrideView( pEcx, pSetupView );
}

float_t __fastcall C_Hooks::hkGetViewmodelFOV( LPVOID pEcx, uint32_t )
{
	return g_Settings->m_iViewmodelDistance;
}