#include "Tools.hpp"
#include "Obfuscation/XorStr.hpp"
#include "../SDK/Globals.hpp"

uint8_t* C_Tools::FindPattern( HMODULE module, std::string strByteArray )
{
    static auto pattern_to_byte = [](const char* pattern) {
        auto bytes = std::vector<int>{};
        auto start = const_cast<char*>(pattern);
        auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current) {
            if (*current == '?') {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else {
                bytes.push_back(strtoul(current, &current, 16));
            }
        }
        return bytes;
    };

    auto dosHeader = (PIMAGE_DOS_HEADER)module;
    auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

    auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = pattern_to_byte(strByteArray.c_str( ));
    auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

    auto s = patternBytes.size();
    auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i) {
        bool found = true;
        for (auto j = 0ul; j < s; ++j) {
            if (scanBytes[ i + j ] != d[ j ] && d[ j ] != -1) {
                found = false;
                break;
            }
        }
        if (found) {
            return &scanBytes[ i ];
        }
    }
    return nullptr;
}

bool C_Tools::IsGameUIActive( )
{
    return false;
}

DWORD* C_Tools::FindHudElement( const char* szHudName )
{
    return ( DWORD* )( ( ( DWORD( __thiscall* )( LPVOID, const char* ) )( g_Globals.m_AddressList.m_FindHudElement ) )( *reinterpret_cast < PDWORD* >( g_Globals.m_AddressList.m_FindHudElement_Ptr ), szHudName ) );
}

#include "../SDK/Globals.hpp"
bool C_Tools::IsBindActive( C_KeyBind KeyBind )
{
    if ( KeyBind.m_iKeySelected <= 0 )
    {
        if ( KeyBind.m_iModeSelected == 3 )
            return true;

        return false;
    }

    switch ( KeyBind.m_iModeSelected )
    {
        case 0:  
            return g_Globals.m_KeyData.m_aToggledKeys[ KeyBind.m_iKeySelected ];
        case 1: 
            return g_Globals.m_KeyData.m_aHoldedKeys[ KeyBind.m_iKeySelected ];
        case 2: 
            return !g_Globals.m_KeyData.m_aHoldedKeys[ KeyBind.m_iKeySelected ];
        case 3: 
            return true; 
    }

    return false;
}

void C_Tools::SetSkybox( const char* szSkybox )
{
    return ( ( void( __fastcall* )( const char* ) )( g_Globals.m_AddressList.m_LoadSkybox ) )( szSkybox );
}

LPVOID C_Tools::GetInterface( HMODULE hModule, const char* InterfaceName )
{
    typedef void* ( *CreateInterfaceFn )( const char*, int* );
	return reinterpret_cast< void* >( reinterpret_cast< CreateInterfaceFn >( GetProcAddress( hModule, _S( "CreateInterface" ) ) )( InterfaceName, NULL ) );
}

int32_t C_Tools::RandomInt( int32_t iMin, int32_t iMax )
{
	return ( ( int32_t(*)( int32_t, int32_t ) )( GetProcAddress( GetModuleHandleA( _S( "vstdlib.dll" ) ), _S( "RandomInt" ) ) ) )( iMin, iMax );
}

float_t C_Tools::RandomFloat( float_t flMin, float_t flMax )
{
	return ( ( float_t(*)( float_t, float_t ) )( GetProcAddress( GetModuleHandleA( _S( "vstdlib.dll" ) ), _S( "RandomFloat" ) ) ) )( flMin, flMax );
}

void C_Tools::RandomSeed( int32_t iSeed )
{
	( reinterpret_cast< void( * )( int32_t ) >( GetProcAddress( GetModuleHandleA( _S( "vstdlib.dll" ) ), _S( "RandomSeed" ) ) ) )( iSeed );
}
