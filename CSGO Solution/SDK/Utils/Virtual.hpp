#pragma once

template < typename T >
static T GetVirtual( void* pClass, int nIndex )
{
	return reinterpret_cast< T >( ( *( int** ) pClass )[ nIndex ] );
}

#define PushVirtual( strFunctionName, nIndex, Type, ...) \
auto strFunctionName { \
return GetVirtual< Type >( this, nIndex )( this, __VA_ARGS__ ); \
}; 