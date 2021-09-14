#pragma once
#include <cstdint>
#include "../Game/NetChannel.hpp"
#include "../Game/EventInfo.hpp"
#include "../Game/ClockDriftMgr.hpp"

#define MAX_USER_CMDS
#define MAX_SIMULATION_TICKS 17
#define NET_FRAMES_BACKUP	64	
#define NET_FRAMES_MASK		( NET_FRAMES_BACKUP-1 )

class C_ClientState
{
public:
	void ForceFullUpdate( )
	{
		m_nDeltaTick( ) = -1;
	}

	C_ClockDriftMgr& m_ClockDriftMgr( )
	{
		return *( C_ClockDriftMgr* )( ( uintptr_t )( this ) + ( 0x0128 ) );
	}

	C_NetChannel* m_pNetChannel( )
	{
		return *( C_NetChannel** )( ( uintptr_t )( this ) + ( 0x009C ) );
	}

	int32_t& m_nChokedCommands( )
	{
		return*( int32_t* )( ( uintptr_t )( this ) + ( 0x4D30 ) );
	}

	int32_t& m_nLastOutgoingCommand( )
	{
		return*( int32_t* )( ( uintptr_t )( this ) + ( 0x4D2C ) );
	}

	int32_t& m_nLastCommandAck( )
	{
		return*( int32_t* )( ( uintptr_t )( this ) + ( 0x4D34 ) );
	}

	int32_t& m_nDeltaTick( )
	{
		return*( int32_t* )( ( uintptr_t )( this ) + ( 0x174 ) );
	}

	int32_t& m_iCommandAck( )
	{
		return *( int32_t* )( ( uintptr_t )( this ) + ( 0x4D38 ) );
	}

	int32_t& GetServerTick( )
	{
		return *( int32_t* )( ( uintptr_t )( this ) + ( 0x16C ) );
	}

	float_t& m_flNextCmdTime( )
	{
		return *( float_t* )( ( uintptr_t )( this ) + ( 0x114 ) );
	}

	C_EventInfo* m_aEvents( )
	{
		return *( C_EventInfo** )( ( uintptr_t )( this ) + ( 0x4E74 ) );
	}
};