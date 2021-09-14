#pragma once
#include "../Utils/Virtual.hpp"

class C_Panel
{
public:
    const char* GetName( unsigned int vguiPanel )
    {
        return GetVirtual < const char* ( __thiscall* )( void*, unsigned int ) > ( this, 36 )( this, vguiPanel );
    }

    const char* GetClassName( unsigned int vguiPanel )
    {
        return GetVirtual < const char* ( __thiscall* )( void*, unsigned int ) > ( this, 37 )( this, vguiPanel );
    }
};
