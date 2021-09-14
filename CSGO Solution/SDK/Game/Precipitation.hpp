#pragma once

class C_Precipitation : public C_BaseEntity
{
public:
	NETVAR( m_nPrecipitationType, int32_t, FNV32( "DT_Precipitation" ), FNV32( "m_nPrecipType" ) );
};