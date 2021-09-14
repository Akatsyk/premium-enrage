#pragma once
#include <array>
#include "Globals.hpp"
#include "Utils/EHandle.hpp"

#include "../Tools/Netvars/Netvars.hpp"
#include "../Tools/Obfuscation/FN1VA.hpp"

#include "Game/DataMap.hpp"
inline uint32_t FindInDataMap( C_DataMap* pDataMap, uint32_t iPropName )
{
    while ( pDataMap )
    {
        for ( int i = 0; i < pDataMap->m_iDataNumFields; i++ )
        {
            if ( !pDataMap->m_pDataDescription[ i ].fieldName )
                continue;

            if ( FNV32( pDataMap->m_pDataDescription[ i ].fieldName ) == iPropName)
                return pDataMap->m_pDataDescription[ i ].fieldOffset[ TD_OFFSET_NORMAL ];

            if ( pDataMap->m_pDataDescription[i].fieldType != FIELD_EMBEDDED )
                continue;

            if ( !pDataMap->m_pDataDescription[ i ].td )
                continue;

            uint32_t iOffset = FindInDataMap( pDataMap->m_pDataDescription[ i ].td, iPropName );
            if ( !iOffset )
                continue;

            return iOffset;
        }

        pDataMap = pDataMap->m_pBaseMap;
    }

    return 0;
}

#ifdef __READY__

#define CUSTOM_OFFSET( name, type, prop, offset ) \
__forceinline type& name( ) \
{\
    volatile uint32_t uRor = 0xDEADC0DE;\
    volatile uint32_t uKey1 = FNV32( "CUSTOM" );\
    volatile uint32_t uKey2 = prop;\
    return *( type* )( ( uintptr_t )( this ) + ( _rotl( uKey2, uRor ) ^ ByteSwap( uKey1 ) ) ); \
}\

#define NETVAR( name, type, table, prop ) \
__forceinline type& name( ) \
{\
    volatile uint32_t uRor = 0xDEADC0DE;\
    volatile uint32_t uKey1 = table;\
    volatile uint32_t uKey2 = prop;\
    return *( type* )( ( uintptr_t )( this ) + ( _rotl( uKey2, uRor ) ^ ByteSwap( uKey1 ) ) ); \
}\

#define PNETVAR(type, name, table, netvar)                           \
    type* name() const {                                          \
        volatile uint32_t uRor = 0xDEADC0DE;\
        volatile uint32_t uKey1 = table;\
        volatile uint32_t uKey2 = netvar;\
        return ( type* )( ( uintptr_t )( this ) + ( _rotl( uKey2, uRor ) ^ ByteSwap( uKey1 ) ) );                  \
    }\


#else

#define CUSTOM_OFFSET( name, type, prop, offset ) \
__forceinline type& name( ) \
{\
    return *( type* )( ( uintptr_t )( this ) + offset ); \
}\

#define NETVAR( name, type, table, prop ) \
__forceinline type& name( ) \
{\
    static uint32_t g_##name = 0; \
    if ( !g_##name )\
        g_##name = g_NetvarManager->GetNetvar( table, prop ); \
    return *( type* )( ( uintptr_t )( this ) + g_##name ); \
}\

#define PNETVAR( type, name, table, netvar )                           \
    type* name() const {                                          \
        static uint32_t g_##name = 0; \
        if ( !g_##name )\
            g_##name = g_NetvarManager->GetNetvar( table, netvar ); \
        return ( type* )( ( uintptr_t )( this ) + g_##name );                  \
    }\

#endif

#define DATAMAP( type, name ) \
__forceinline type& name( ) \
{\
	static uint32_t g_Datamap_##name = 0;\
    if ( !g_Datamap_##name )\
        g_Datamap_##name = _rotl( FindInDataMap( this->GetPredDescMap( ), FNV32( #name ) ) ^ ( FNV32( #name ) * 5 ), 32 );\
    return *( type* )( ( uintptr_t )( this ) + ( _rotr( g_Datamap_##name, 32 ) ^ ( FNV32( #name ) * 5 ) ) );\
}\

#include "Game/BaseEntity.hpp"
#include "Game/BaseCombatWeapon.hpp"
#include "Game/BaseViewmodel.hpp"
#include "Game/BasePlayer.hpp"
#include "Game/Precipitation.hpp"