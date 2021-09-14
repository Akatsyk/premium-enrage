#include "../Hooks.hpp"

void __fastcall C_Hooks::hkProcessMovement( LPVOID pEcx, uint32_t, C_BasePlayer* pPlayer, C_MoveData* pMoveData )
{
	pMoveData->m_bGameCodeMovedPlayer = false;
	return g_Globals.m_Hooks.m_Originals.m_ProcessMovement( pEcx, pPlayer, pMoveData );
}

bool __fastcall C_Hooks::hkTraceFilterForHeadCollision( LPVOID pEcx, uint32_t, C_BasePlayer* pPlayer, LPVOID pTraceParams )
{
	if ( !g_Globals.m_LocalPlayer || !g_Globals.m_LocalPlayer->IsAlive( ) )
		return g_Globals.m_Hooks.m_Originals.m_TraceFilterForHeadCollision( pEcx, pPlayer, pTraceParams );

	if ( !pPlayer || !pPlayer->IsPlayer( ) || pPlayer->EntIndex( ) - 1 > 63 || pPlayer == g_Globals.m_LocalPlayer )
		return g_Globals.m_Hooks.m_Originals.m_TraceFilterForHeadCollision( pEcx, pPlayer, pTraceParams );

	if ( fabsf( pPlayer->m_vecOrigin( ).z - g_Globals.m_LocalPlayer->m_vecOrigin( ).z ) < 10.0f )
		return false;

	return g_Globals.m_Hooks.m_Originals.m_TraceFilterForHeadCollision( pEcx, pPlayer, pTraceParams );
}