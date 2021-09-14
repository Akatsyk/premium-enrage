#pragma once
#include "../Utils/Virtual.hpp"

class C_FileSystem
{
public:
    int Read( void* output, int size, void* file ) 
    {
        typedef int( __thiscall* tRead )( void*, void*, int, void* );
        return GetVirtual< tRead >( this, 0 )( this, output, size, file );
    }

    void* Open( const char* file_name, const char* options, const char* path_id ) 
    {
        typedef void*( __thiscall* tOpen )( void*, const char*, const char*, const char* );
        return GetVirtual< tOpen >( this, 2 )( this, file_name, options, path_id );
    }

    void Close( void* file)
    {
        typedef void( __thiscall* tClose )( void*, void* );
        return GetVirtual< tClose >( this, 3 )( this, file );
    }

    unsigned int Size( void* file) 
    {
        typedef unsigned int( __thiscall* tSize )( void*, void* );
        return GetVirtual< tSize >( this, 7 )( this, file );
    }
};