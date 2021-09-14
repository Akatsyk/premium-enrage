#include "../Hooks.hpp"
#include "../Render.hpp"
#include "../Features/Model/Model.hpp"
#include "../Features/Visuals/World.hpp"

void __fastcall C_Hooks::hkLockCursor( LPVOID pEcx, uint32_t )
{
	if ( g_Menu->IsMenuOpened( ) )
		return g_Globals.m_Interfaces.m_Surface->UnlockCursor( );

	return g_Globals.m_Hooks.m_Originals.m_LockCursor( pEcx );
}

void __fastcall C_Hooks::hkPaintTraverse( LPVOID pEcx, uint32_t, VGUI::VPANEL pPanel, bool bForceRepaint, bool bAllowForce )
{
	if ( strstr( g_Globals.m_Interfaces.m_VGUIPanel->GetName( pPanel ), _S( "HudZoom" ) ) )
		if ( g_Settings->m_aWorldRemovals[ REMOVALS_VISUAL_SCOPE ] )
			return;

	g_Globals.m_Hooks.m_Originals.m_PaintTraverse( pEcx, pPanel, bForceRepaint, bAllowForce );

	static VGUI::VPANEL pPanelID = NULL;
	if ( !pPanelID )  
	{
		if ( !strcmp( g_Globals.m_Interfaces.m_VGUIPanel->GetName( pPanel ), _S( "FocusOverlayPanel" ) ) )
			pPanelID = pPanel;
		
	}
	else if ( pPanelID == pPanel )
	{
		g_World->PreserveKillfeed( );
		if ( g_Menu->IsMenuOpened( ) )
			g_DrawModel->Instance( );

		g_Render->Instance( );
	}
}