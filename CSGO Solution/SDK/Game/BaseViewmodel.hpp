#pragma once

class C_BaseViewModel : public C_BaseEntity
{
public:
	NETVAR( m_hWeapon, CHandle< C_BaseCombatWeapon >, FNV32( "DT_BaseViewModel" ), FNV32( "m_hWeapon" ) );

	NETVAR( m_iSequence, int, FNV32( "DT_BaseViewModel" ), FNV32( "m_nSequence" ) );
	NETVAR( m_iAnimationParity, int, FNV32( "DT_BaseViewModel" ), FNV32( "m_nAnimationParity" ) );

	DATAMAP( float_t, m_flCycle );
	DATAMAP( float_t, m_flAnimTime );
};