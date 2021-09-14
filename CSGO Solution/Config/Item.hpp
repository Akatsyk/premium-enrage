#pragma once
#include <any>
#include <cstdint>
#include "../SDK/Utils/Color/Color.hpp"

struct C_ConfigItem 
{
	C_ConfigItem( const std::string name, const std::string type, const std::any var )
		: Name( name ), Type( type ), Var( var )
	{ };

	template <typename T>
	T& Get( )
	{
		return *reinterpret_cast<T*>(std::any_cast<T>(&Var));
	}

	template <typename T>
	T* GetPtr( )
	{
		return reinterpret_cast<T*>(std::any_cast<T>(&Var));
	}

	template <typename T>
	void Set( T val )
	{
		Var.emplace<T>(val);
	}

	std::string Name;
	std::string Type;
	std::any Var;
};


struct C_Binds
{
	std::string Name;
	int Index;

	C_Binds(const std::string name, int id)
		: Name(name), Index(id)
	{ };
};
