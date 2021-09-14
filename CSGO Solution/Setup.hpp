#pragma once
#include "SDK/Globals.hpp"

class C_SetUp
{
public:
	static void Instance( HINSTANCE hModule );
	static void SetupAddressList( );
	static void SetupModuleList( );
	static void SetupInterfaceList( );
	static void SetupConvarList( );
	static void SetupHooks( );
	static void SetupImGui( );
};