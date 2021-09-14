#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include "../Settings.hpp"

class C_Tools
{
public:
	virtual uint8_t* FindPattern( HMODULE hModule, std::string strByteArray );
	virtual LPVOID GetInterface( HMODULE hModule, const char* InterfaceName );
	
	virtual DWORD* FindHudElement( const char* szHudName );
	virtual bool IsBindActive( C_KeyBind KeyBind );
	virtual bool IsGameUIActive( );
	virtual void SetSkybox( const char* szSkybox );

	virtual void RandomSeed( int32_t iSeed );
	virtual float_t RandomFloat( float_t flMin, float_t flMax );
	virtual int32_t RandomInt( int32_t iMin, int32_t iMax );
};

inline C_Tools* g_Tools = new C_Tools( );