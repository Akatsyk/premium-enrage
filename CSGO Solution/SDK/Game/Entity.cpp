#include "../Includes.hpp"

void C_BaseEntity::SetAbsoluteAngles( QAngle angViewAngles )
{
	return ( ( void( __thiscall* )( LPVOID, QAngle& ) )( g_Globals.m_AddressList.m_SetAbsAngles ) )( this, angViewAngles );
}

void C_BaseEntity::SetAbsoluteOrigin( Vector vetAbsOrigin )
{
	return ( ( void( __thiscall* )( LPVOID, Vector& ) )( g_Globals.m_AddressList.m_SetAbsOrigin ) )( this, vetAbsOrigin );
}

void C_BaseEntity::SetWorldOrigin( Vector vecWorldOrigin )
{
	this->m_vecOrigin( ) = vecWorldOrigin;
}