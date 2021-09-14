#include "../Hooks.hpp"
#include "../Render.hpp"
#include "../Tools/Tools.hpp"

long WINAPI C_Hooks::hkPresent( IDirect3DDevice9* pDevice, RECT* pSrcRect, RECT* pDescRect, HWND pHWND, RGNDATA* pRegion )
{
	IDirect3DVertexDeclaration9* pVertDec;
	if ( pDevice->GetVertexDeclaration( &pVertDec ) )
		return g_Globals.m_Hooks.m_Originals.m_Present( pDevice, pSrcRect, pDescRect, pHWND, pRegion );
	
	IDirect3DVertexShader9* pVertShader;
	if ( pDevice->GetVertexShader( &pVertShader ) )
		return g_Globals.m_Hooks.m_Originals.m_Present( pDevice, pSrcRect, pDescRect, pHWND, pRegion );

	g_Render->ReloadFonts( );
	ImGui_ImplDX9_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	auto DrawList = g_Render->GetDrawList( );
	g_Menu->Instance( );
	ImGui::Render( DrawList );
	if ( pDevice->BeginScene( ) == D3D_OK )
	{
		ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
		pDevice->EndScene( );
	}
  
	pDevice->SetVertexShader( pVertShader );
	pDevice->SetVertexDeclaration( pVertDec );

	return g_Globals.m_Hooks.m_Originals.m_Present( pDevice, pSrcRect, pDescRect, pHWND, pRegion );
}

long WINAPI C_Hooks::hkReset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pDeviceParameters )
{
	ImGui_ImplDX9_InvalidateDeviceObjects( );
	long lResult = g_Globals.m_Hooks.m_Originals.m_Reset( pDevice, pDeviceParameters );
	ImGui_ImplDX9_CreateDeviceObjects( );

	return lResult;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT WINAPI C_Hooks::hkWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	bool bIsHandlePossible = true;
	if ( g_Globals.m_Interfaces.m_EngineClient->IsConnected( ) && g_Globals.m_Interfaces.m_EngineClient->IsInGame( ) )
	{
		if ( g_Tools->IsGameUIActive( ) || g_Globals.m_Interfaces.m_EngineClient->Con_IsVisible( ) )
		{
			bIsHandlePossible = false;
			if ( msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP || msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP || msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP )
				bIsHandlePossible = true;

			if ( msg == WM_KEYDOWN || msg == WM_KEYUP )
				if ( wParam == VK_INSERT )
					bIsHandlePossible = true;
		}
	}

	if ( bIsHandlePossible )
	{
		switch ( msg )
		{
			case WM_LBUTTONDOWN:
				g_Globals.m_KeyData.m_aToggledKeys[ VK_LBUTTON ] = !g_Globals.m_KeyData.m_aToggledKeys[ VK_LBUTTON ];
				g_Globals.m_KeyData.m_aHoldedKeys[ VK_LBUTTON ] = true;
				break;
			case WM_LBUTTONUP:
				g_Globals.m_KeyData.m_aHoldedKeys[ VK_LBUTTON ] = false;
				break;
			case WM_XBUTTONDOWN:
				if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 )
				{
					g_Globals.m_KeyData.m_aHoldedKeys[ VK_XBUTTON1 ] = true;
					g_Globals.m_KeyData.m_aToggledKeys[ VK_XBUTTON1 ] = !g_Globals.m_KeyData.m_aToggledKeys[ VK_XBUTTON1 ];
				}

				if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON2 )
				{
					g_Globals.m_KeyData.m_aHoldedKeys[ VK_XBUTTON2 ] = true;
					g_Globals.m_KeyData.m_aToggledKeys[ VK_XBUTTON2 ] = !g_Globals.m_KeyData.m_aToggledKeys[ VK_XBUTTON2 ];
				}
				break;
			case WM_XBUTTONUP:
				if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 )
					g_Globals.m_KeyData.m_aHoldedKeys[ VK_XBUTTON1 ] = false;

				if ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON2 )
					g_Globals.m_KeyData.m_aHoldedKeys[ VK_XBUTTON2 ] = false;
				break;
			case WM_MBUTTONDOWN:
				g_Globals.m_KeyData.m_aHoldedKeys[ VK_MBUTTON ] = true;
				g_Globals.m_KeyData.m_aToggledKeys[ VK_MBUTTON ] = !g_Globals.m_KeyData.m_aToggledKeys[ VK_MBUTTON ];
				break;
			case WM_MBUTTONUP:
				g_Globals.m_KeyData.m_aHoldedKeys[ VK_MBUTTON ] = false;
				break;
			case WM_RBUTTONDOWN:
				g_Globals.m_KeyData.m_aToggledKeys[ VK_RBUTTON ] = !g_Globals.m_KeyData.m_aToggledKeys[ VK_RBUTTON ];
				g_Globals.m_KeyData.m_aHoldedKeys[ VK_RBUTTON ] = true;
				break;
			case WM_RBUTTONUP:
				g_Globals.m_KeyData.m_aHoldedKeys[ VK_RBUTTON ] = false;
				break;
			case WM_KEYDOWN:
				g_Globals.m_KeyData.m_aToggledKeys[ wParam ] = !g_Globals.m_KeyData.m_aToggledKeys[ wParam ];
				g_Globals.m_KeyData.m_aHoldedKeys[ wParam ] = true;
				break;
			case WM_KEYUP:
				g_Globals.m_KeyData.m_aHoldedKeys[ wParam ] = false;
				break;
			default: break;
		}
	}

	if ( !g_Globals.m_LocalPlayer )
	{
		for ( int i = 0; i < 256; i++ )
		{
			if ( i == VK_INSERT )
				continue;

			g_Globals.m_KeyData.m_aHoldedKeys[ i ] = false;
			g_Globals.m_KeyData.m_aToggledKeys[ i ] = false;
		}
	}

	// теперь объ€сн€ю как это использовать
	// √де нахуй угодно находимс€ и если нужно проверить какую-то клавишу вызываем 
	// g_Globals.m_KeyData.m_aHoldedKeys[ wParam ] ( если нужно проверить удерживаетс€ клавиша )
	// g_Globals.m_KeyData.m_aToggledKeys[ wParam ] ( если нужно проверить нажата ли клавиша ( автоматически ) )

	bool bPressedMovementKeys = false;
	if ( msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP || msg == WM_LBUTTONUP || msg == WM_MOUSEMOVE || msg == WM_MOUSEWHEEL )
		bPressedMovementKeys = true;
	
	if ( ImGui::GetIO().WantTextInput )
		bPressedMovementKeys = false;

	g_Menu->SetMenuState( g_Globals.m_KeyData.m_aToggledKeys[ VK_INSERT ] );
	if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) && g_Menu->IsMenuOpened( ) && !bPressedMovementKeys )
		return true;

	if ( g_Menu->IsMenuOpened( ) && bPressedMovementKeys )
		return false;

	return CallWindowProc( ( WNDPROC )( g_Globals.m_Render.m_OldWndProc ), hWnd, msg, wParam, lParam );
}