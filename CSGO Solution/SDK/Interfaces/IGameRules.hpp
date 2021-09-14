#pragma once
#include <cstdint>

class C_GameRules
{
public:
	bool IsFreezePeriod( ) 
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x20 );
	}

	bool IsValveDS( ) 
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x7C );
	}

	bool IsBombPlanted( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x8D1 );
	}

};