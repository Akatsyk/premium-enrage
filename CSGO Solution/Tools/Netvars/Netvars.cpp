#include "Netvars.hpp"
#include "../../Tools/Obfuscation/FN1VA.hpp"
#include "../../SDK/Globals.hpp"

void C_NetVarManager::Instance( )
{
	for ( auto pClass = g_Globals.m_Interfaces.m_CHLClient->GetAllClasses( ); pClass != nullptr; pClass = pClass->m_pNext )
	{
		if ( !pClass || !pClass->m_pRecvTable )
			continue;

		m_aNetVars.emplace_back( this->ScanTable( ( RecvTable* )( pClass->m_pRecvTable ) ) );
	}
}

uint32_t C_NetVarManager::GetNetvar( uint32_t uTableName, uint32_t uPropName )
{
	for ( auto& Table : m_aNetVars )
	{
		if ( FNV32( Table.m_strName.c_str( ) ) != uTableName )
			continue;

		return GetOffset( Table, uPropName );
	}
}

RecvProp* C_NetVarManager::GetNetProp( std::string strTableName, std::string strPropName )
{
	for ( auto& Table : m_aNetVars )
	{
		if ( Table.m_strName.c_str( ) != strTableName )
			continue;

		return GetNetvarProp( Table, strPropName );
	}
}

inline C_NetVar C_NetVarManager::ScanTable( RecvTable* pTable )
{
	C_NetVar aNetVars;
	aNetVars.m_uOffset = 0;
	aNetVars.m_strName = pTable->m_pNetTableName;

	for ( int32_t i = 0; i < pTable->m_nProps; i++ )
	{
		auto Prop = &pTable->m_pProps[ i ];
		if ( !Prop || isdigit( Prop->m_pVarName[ 0 ] ) )
			continue;

		if ( strstr( _S( "baseclass" ), Prop->m_pVarName ) )
			continue;

		if ( Prop->m_RecvType != DPT_DataTable || 
			!Prop->m_pDataTable )
		{
			aNetVars.m_aChildProps.emplace_back( Prop );
			continue;
		}

		aNetVars.m_aChildTables.emplace_back( ScanTable( Prop->m_pDataTable ) );
		aNetVars.m_aChildTables.back( ).m_uOffset = Prop->m_Offset;
		aNetVars.m_aChildTables.back( ).m_pProp = Prop;
	}

	return aNetVars;
}

inline uint32_t C_NetVarManager::GetOffset( const C_NetVar& pTable, const uint32_t uPropName )
{
	for ( auto& Prop : pTable.m_aChildProps )
	{
		if ( FNV32( Prop->m_pVarName ) != uPropName )
			continue;

		return pTable.m_uOffset + Prop->m_Offset;
	}

	for ( auto& Child : pTable.m_aChildTables )
	{
		uint32_t uOffset = GetOffset( Child, uPropName );
		if ( !uOffset )
			continue;

		return pTable.m_uOffset + uOffset;
	}

	for ( auto& Child : pTable.m_aChildTables )
	{
		if ( FNV32( Child.m_pProp->m_pVarName ) != uPropName )
			continue;

		return pTable.m_uOffset + Child.m_pProp->m_Offset;
	}
}

inline RecvProp* C_NetVarManager::GetNetvarProp( const C_NetVar& pTable, std::string strPropName )
{
	for ( auto& Prop : pTable.m_aChildProps )
	{
		if ( Prop->m_pVarName != strPropName )
			continue;

		return Prop;
	}

	for ( auto& Child : pTable.m_aChildTables )
	{
		RecvProp* Prop = GetNetvarProp( Child, strPropName );
		if ( !Prop )
			continue;

		return Prop;
	}

	for ( auto& Child : pTable.m_aChildTables )
	{
		if ( Child.m_pProp->m_pVarName != strPropName )
			continue;

		return Child.m_pProp;
	}
}