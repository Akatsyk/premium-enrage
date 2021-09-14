#pragma once
#include <any>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>

#include "Config/Item.hpp"

class C_ConfigSystem
{
public:
	template <typename T>
	uint32_t PushItem( const std::string name, const std::string type, const T default_value )
	{
		m_aItems.emplace_back( C_ConfigItem( name, type, std::make_any< T >( default_value ) ) );
		return m_aItems.size( ) - 1u;
	}

	template <typename T>
	T* GetConfigValue( unsigned int idx )
	{
		return &m_aItems[ idx ].Get< T >( );
	}

	virtual void SaveConfig( );
	virtual void LoadConfig( );
private:
	std::vector < C_ConfigItem > m_aItems;
};

inline C_ConfigSystem* g_ConfigSystem = new C_ConfigSystem( );