#include <Windows.h>
#include "KeyValues.hpp"
#include "../Globals.hpp"

typedef C_KeyValues* (__thiscall* FindKey_t)(void*, const char*, bool);
typedef const char* (__thiscall* GetString_t)(void*, const char*, const char*);
typedef bool(__thiscall* LoadFromBuffer_t)(C_KeyValues*, const char*, const char*, void*, void*, void*, void*);
typedef void(__thiscall* SetString_t)(C_KeyValues*, const char*);

void* C_KeyValues::operator new( size_t allocatedsize )
{
    static PVOID pKeyValuesSystem;
    if ( !pKeyValuesSystem )
        pKeyValuesSystem = ( reinterpret_cast< PVOID ( __cdecl* )( ) >( GetProcAddress( GetModuleHandleA( _S( "vstdlib.dll" ) ), _S( "KeyValuesSystem" ) ) ) )( );

    return GetVirtual< PVOID( __thiscall* )( PVOID, size_t ) >( pKeyValuesSystem, 1 )( pKeyValuesSystem, allocatedsize );
}

void C_KeyValues::operator delete( void* mem )
{
    static PVOID pKeyValuesSystem;
    if (!pKeyValuesSystem) 
        pKeyValuesSystem = ( reinterpret_cast< PVOID ( __cdecl* )( ) >( GetProcAddress( GetModuleHandleA( _S( "vstdlib.dll" ) ), _S( "KeyValuesSystem" ) ) ) )( );

    return GetVirtual< void( __thiscall* )( PVOID, PVOID ) >( pKeyValuesSystem, 2 )( pKeyValuesSystem, mem );
}

void C_KeyValues::Init( )
{
    m_iKeyName = -1;
    m_iDataType = TYPE_NONE;

    m_pSub = NULL;
    m_pPeer = NULL;
    m_pChain = NULL;

    m_sValue = NULL;
    m_wsValue = NULL;
    m_pValue = NULL;

    m_bHasEscapeSequences = false;

    memset( unused, 0, sizeof( unused ) );
}

C_KeyValues* C_KeyValues::FindKey( const char* strKeyName, bool bCreate ) 
{
    return ( ( FindKey_t )( g_Globals.m_AddressList.m_KeyValuesFindKey ) )( this, strKeyName, bCreate );
}

const char* C_KeyValues::GetString( C_KeyValues* pThis, const char* strKeyName, const char* strDefaultValue )
{
    return ( ( GetString_t )( g_Globals.m_AddressList.m_KeyValuesGetString ) )( pThis, strKeyName, strDefaultValue );
}

bool C_KeyValues::LoadFromBuffer( C_KeyValues* pThis, const char* pszFirst, const char* pszSecond, PVOID pSomething, PVOID pAnother, PVOID pLast, PVOID pAnother2 )
{
    return ( ( LoadFromBuffer_t )( g_Globals.m_AddressList.m_KeyValuesLoadFromBuffer ) )( pThis, pszFirst, pszSecond, pSomething, pAnother, pLast, pAnother2 );
}

void C_KeyValues::SetString( const char* strName, const char* strValue ) 
{
    C_KeyValues* pKeyValues = this->FindKey( strName, 1 );
    if ( pKeyValues )
        ( ( SetString_t )( g_Globals.m_AddressList.m_KeyValuesSetString ) )( pKeyValues, strValue );
}

void C_KeyValues::SetInt( const char* strName, int iValue ) 
{
    auto pKeyValues = FindKey( strName, 1 );
    if ( !pKeyValues )
        return;

    m_iValue = iValue;
    m_iDataType = 2;
}

void C_KeyValues::SetFloat( const char* strName, float flValue ) 
{
    auto pKeyValues = FindKey( strName, 1 );
    if ( !pKeyValues )
        return;

    m_flValue = flValue;
    m_iDataType = 3;
}