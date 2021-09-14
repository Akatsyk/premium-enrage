#pragma once

class C_BaseEntity : public C_ClientEntity
{
public:
	PushVirtual( GetPredDescMap( ), 17, C_DataMap*( __thiscall* )( void* ) );
	PushVirtual( IsPlayer( ), 157, bool( __thiscall* )( void* ) );
	PushVirtual( SetModelIndex( int i ), 75, void( __thiscall* )( void*, int ), i );

	NETVAR( m_flSimulationTime		,	float_t	, FNV32( "DT_BaseEntity" ), FNV32( "m_flSimulationTime" ) );
	NETVAR( m_iTeamNum				,	int32_t	, FNV32( "DT_BaseEntity" ), FNV32( "m_iTeamNum" ) );
	NETVAR( m_vecOrigin				,	Vector	, FNV32( "DT_BaseEntity" ), FNV32( "m_vecOrigin" ) );
#ifdef __READY__
	NETVAR( m_nRenderMode			,	int32_t	, FNV32( "DT_BaseEntity" ), FNV32( "m_nRenderMode" ) );
#endif

	void SetAbsoluteAngles( QAngle angViewAngle );
	void SetAbsoluteOrigin( Vector vecAbsOrigin );
	void SetWorldOrigin( Vector vecWorldOrigin );

	DATAMAP( int32_t	,	m_iEFlags			);
	DATAMAP( float_t	,	m_flStamina			);
	DATAMAP( float_t	,	m_flCycle			);
	DATAMAP( int32_t	,	m_fEffects			);
	DATAMAP( Vector		,	m_vecAbsVelocity	);
	DATAMAP( Vector		,	m_vecBaseVelocity	);

	bool IsBreakableEntity( )
	{
		int32_t m_TakeDamage = *( int32_t* )( ( uintptr_t )( this ) + 0x280 );
		if ( !strcmp( this->GetClientClass( )->m_strNetworkName, _S( "CBreakableSurface" ) ) )
			*( int32_t* )( ( uintptr_t )( this ) + 0x280 ) = 2;
		else if ( !strcmp( this->GetClientClass( )->m_strNetworkName, _S( "CBaseDoor" ) ) || !strcmp( this->GetClientClass( )->m_strNetworkName, _S( "CDynamicProp" ) ))
			*( int32_t* )( ( uintptr_t )( this ) + 0x280 ) = 0;

		bool bIsBreakableEntity = ( ( bool( __thiscall* )( C_BaseEntity* ) )( g_Globals.m_AddressList.m_IsBreakableEntity ) )( this );
		
		*( int32_t* )( ( uintptr_t )( this ) + 0x280 ) = m_TakeDamage;
		return bIsBreakableEntity;
	}

	CUSTOM_OFFSET( m_rgflCoordinateFrame, matrix3x4_t, FNV32( "CoordinateFrame" ), 1092 );
	CUSTOM_OFFSET( m_flOldSimulationTime, float_t, FNV32( "OldSimulationTime" ), 620 );
	CUSTOM_OFFSET( m_flFlashTime, float_t, FNV32( "FlashTime" ), 42000 );
	CUSTOM_OFFSET( m_nExplodeEffectTickBegin, int32_t, FNV32( "m_nExplodeEffectTickBegin" ), 10724 );
	CUSTOM_OFFSET( m_flCreationTime, float_t, FNV32( "CreationTime" ), 10740 );
	CUSTOM_OFFSET( m_vecMins, Vector, FNV32( "Mins" ), 808 );
	CUSTOM_OFFSET( m_vecMaxs, Vector, FNV32( "Maxs" ), 820 );
#ifndef __READY__
	CUSTOM_OFFSET( m_nRenderMode, int32_t, FNV32( "RenderMode" ), 604 );
#endif
};