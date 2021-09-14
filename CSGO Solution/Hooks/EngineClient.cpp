#include "../Hooks.hpp"
#include "../Settings.hpp"

bool __fastcall C_Hooks::hkIsHLTV( LPVOID pEcx, uint32_t )
{
	if ( reinterpret_cast < DWORD > ( _ReturnAddress( ) ) == reinterpret_cast < DWORD > ( g_Globals.m_AddressList.m_SetupVelocity_Call ) )
		return true;

	if ( reinterpret_cast < DWORD > ( _ReturnAddress( ) ) == reinterpret_cast < DWORD > ( g_Globals.m_AddressList.m_AccumulateLayers_Call ) )
		return true;

	return g_Globals.m_Hooks.m_Originals.m_IsHLTV( pEcx );
}

bool __fastcall C_Hooks::hkIsPaused( LPVOID pEcx, uint32_t )
{
	if ( reinterpret_cast < DWORD > ( _ReturnAddress( ) ) == reinterpret_cast < DWORD > ( g_Globals.m_AddressList.m_IsPaused_Call ) )
		return true;

	return g_Globals.m_Hooks.m_Originals.m_IsPaused( pEcx );
}

void __fastcall C_Hooks::hkFireEvents( LPVOID pEcx, uint32_t )
{
	for ( C_EventInfo* Event = g_Globals.m_Interfaces.m_ClientState->m_aEvents( ); Event != nullptr; Event = Event->m_pNext )
		Event->m_flFireDelay = 0.0f;

	return g_Globals.m_Hooks.m_Originals.m_FireEvents( pEcx );
}

bool __fastcall C_Hooks::hkIsConnected( LPVOID pEcx, uint32_t )
{
	if ( g_Settings->m_bUnlockInventoryAccess )
		if ( reinterpret_cast < DWORD > ( _ReturnAddress( ) ) == reinterpret_cast < DWORD > ( g_Globals.m_AddressList.m_IsConnected_Call ) )
			return true;

	return g_Globals.m_Hooks.m_Originals.m_IsConnected( pEcx );
}

float_t __fastcall C_Hooks::hkGetScreenSizeAspectRatio( LPVOID pEcx, uint32_t, int32_t X, int32_t Y )
{
	return g_Settings->m_flAspectRatio;
}

int32_t __fastcall C_Hooks::hkListLeavesInBox( LPVOID pEcx, uint32_t, Vector& vecMin, Vector& vecMax, unsigned short* pList, int32_t iMax )
{
	if ( *( uint32_t* )( _ReturnAddress( ) ) != 0x14244489 ) 
		return g_Globals.m_Hooks.m_Originals.m_ListLeavesInBox( pEcx, vecMin, vecMax, pList, iMax );
 
	RenderableInfo_t* pRenderableInfo = *( RenderableInfo_t** )( ( uintptr_t ) _AddressOfReturnAddress( ) + 0x14 );
	if ( !pRenderableInfo || !pRenderableInfo->m_pRenderable )
		return g_Globals.m_Hooks.m_Originals.m_ListLeavesInBox( pEcx, vecMin, vecMax, pList, iMax );

	C_BaseEntity* pBaseEntity = pRenderableInfo->m_pRenderable->GetIClientUnknown( )->GetBaseEntity( );
	if ( !pBaseEntity || !pBaseEntity->IsPlayer( ) )
		return g_Globals.m_Hooks.m_Originals.m_ListLeavesInBox( pEcx, vecMin, vecMax, pList, iMax );

	pRenderableInfo->m_Flags &= ~0x100;
	pRenderableInfo->m_Flags2 |= 0xC0;
    
	return g_Globals.m_Hooks.m_Originals.m_ListLeavesInBox( pEcx, Vector( -16384.0f, -16384.0f, -16384.0f ), Vector( 16384.0f, 16384.0f, 16384.0f ), pList, iMax );
}