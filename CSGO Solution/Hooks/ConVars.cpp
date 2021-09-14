#include "../Hooks.hpp"

bool __fastcall C_Hooks::hkClDoResetLatch_GetBool( LPVOID pEcx, uint32_t )
{
	if ( reinterpret_cast < DWORD >( _ReturnAddress( ) ) == reinterpret_cast < DWORD >( g_Globals.m_AddressList.m_ClDoResetLatch_GetBool_Call ) )
		return false;

	return g_Globals.m_Hooks.m_Originals.m_ClDoResetLatch_GetBool( pEcx );
}

bool __fastcall C_Hooks::hkSvCheats_GetBool( LPVOID pEcx, uint32_t )
{
	if ( reinterpret_cast < DWORD >( _ReturnAddress( ) ) == reinterpret_cast < DWORD >( g_Globals.m_AddressList.m_SvCheats_GetBool_Call ) )
		return true;

	return g_Globals.m_Hooks.m_Originals.m_SvCheats_GetBool( pEcx );
}