#pragma once
#include <map>
#include "Blur/PostProcessing.h"
#include "ImGui/imgui_freetype.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"
#include "Tools/Obfuscation/XorStr.hpp"
#pragma comment( lib, "freetype.lib" )

#include <d3dx9.h>
#include "SDK/Utils/Color/Color.hpp"

struct Bind_t
{
	std::string m_szName = "";
	float_t m_flAlphaPercent = 0.0f;
};

class C_Menu
{
public:
	virtual void Instance( );
	virtual void Initialize( );

	virtual void DrawRageTab( );
	virtual void DrawAntiAimTab( );
	virtual void DrawMiscTab( );
	virtual void DrawPlayersTab( );
	virtual void DrawWorldTab( ); 
	virtual void DrawConfigTab( ); 
	virtual void DrawColorEdit4( const char* strLabel, Color* aColor );

	virtual void SetMenuState( bool bState ) { this->m_bIsMenuOpened = bState; };
	virtual bool IsMenuOpened( ) { return this->m_bIsMenuOpened; };
	virtual IDirect3DTexture9* GetTexture( ) { return this->m_dTexture; };
	virtual const char* GetMenuName( ) { return m_strMenuName.c_str( ); };

	virtual void WaterMark( );
	virtual void DrawKeybindList( );

private:
	bool m_bIsMenuOpened = false;
	int32_t m_iCurrentTab = 0;
	IDirect3DTexture9* m_dTexture = nullptr;
	std::string m_strMenuName = _S( "NOTHING" );

	std::map < uint32_t, Bind_t > m_BindList;
};

inline C_Menu* g_Menu = new C_Menu( );