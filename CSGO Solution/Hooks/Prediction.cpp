#include "../Hooks.hpp"

bool __fastcall C_Hooks::hkInPrediction( LPVOID pEcx, uint32_t )
{
	if ( reinterpret_cast < DWORD >( _ReturnAddress( ) ) == reinterpret_cast < DWORD >( g_Globals.m_AddressList.m_InPrediction_Call ) )
		return false;

	return g_Globals.m_Hooks.m_Originals.m_InPrediction( pEcx );
}