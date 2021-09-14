#pragma once
#include "../Utils/Virtual.hpp"
#include "IGameMovement.hpp"
#include "IMoveHelper.hpp"
#include "../Game/MoveData.hpp"

class C_BasePlayer;
class C_UserCmd;
class C_Prediction
{
public:
	bool& m_bInPrediction( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x8 );
	}

	bool& m_bIsFirstTimePredicted( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x18 );
	}

	bool& m_bIsEnginePaused( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0xA );
	}

	bool& m_bPreviousAckHadErrors( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x24 );
	}

	int32_t& m_iPredictedCommands( )
	{
		return *( int32_t* )( ( uintptr_t )( this ) + 0x1C );
	}

	int32_t& m_iPreviousStartFrame( )
	{
		return *( int32_t* )( ( uintptr_t )( this ) + 0xC );
	}

	int32_t& m_nServerCommandsAcknowledged( )
	{
		return *( int32_t* )( ( uintptr_t )( this ) + 0x20 );
	}

	bool& m_bPerformedTickShift( )
	{
		return *( bool* )( ( uintptr_t )( this ) + 0x48 );
	}

	void ForceRepredict( )
	{
		this->m_iPreviousStartFrame( ) = -1;
		this->m_iPredictedCommands( ) = 0;
	}

	void SetupMove(C_BasePlayer* player, C_UserCmd* ucmd, C_MoveHelper* moveHelper, void* pMoveData)
	{
		typedef void(__thiscall* oSetupMove)(void*, C_BasePlayer*, C_UserCmd*, C_MoveHelper*, void*);
		return GetVirtual<oSetupMove>(this, 20)(this, player, ucmd, moveHelper, pMoveData);
	}

	void FinishMove(C_BasePlayer* player, C_UserCmd* ucmd, void* pMoveData)
	{
		typedef void(__thiscall* oFinishMove)(void*, C_BasePlayer*, C_UserCmd*, void*);
		return GetVirtual<oFinishMove>(this, 21)(this, player, ucmd, pMoveData);
	}

	// vfuncs
	PushVirtual( PostNetworkDataReceived( int32_t iCommandsAck ), 5, void( __thiscall* )( void*, int32_t ), iCommandsAck );
	PushVirtual( Update( int iServerTick, bool bIsValid, int iAcknowledged, int iOutgoingCmd ), 3, void( __thiscall* )( void*, int, bool, int, int ), iServerTick, bIsValid, iAcknowledged, iOutgoingCmd );
	PushVirtual( SetupMove( C_BasePlayer* pPlayer, C_UserCmd* pCmd, C_MoveHelper* pMoveHelper, C_MoveData* MoveData ), 20, void( __thiscall* )( void*, void*, void*, void*, C_MoveData* ), pPlayer, pCmd, pMoveHelper, MoveData );
	PushVirtual( FinishMove( C_BasePlayer* pPlayer, C_UserCmd* pCmd, C_MoveData* MoveData ), 21, void( __thiscall* )( void*, void*, void*, C_MoveData* ), pPlayer, pCmd, MoveData );
	PushVirtual( RunCommand( C_BasePlayer* pPlayer, C_UserCmd* pCmd, C_MoveHelper* pMoveHelper ), 19, void( __thiscall* )( void*, void*, void*, C_MoveHelper* ), pPlayer, pCmd, pMoveHelper );
};