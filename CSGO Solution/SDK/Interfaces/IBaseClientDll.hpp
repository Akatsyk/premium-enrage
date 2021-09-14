#pragma once
#include <cstdint>
#include "../Utils/ClientClass.hpp"
#include "../Utils/Virtual.hpp"

class C_BaseClientDLL
{
public:
	virtual int              Connect(uint32_t appSystemFactory, void* pGlobals) = 0;
    virtual int              Disconnect(void) = 0;
    virtual int              Init(uint32_t appSystemFactory, void* pGlobals) = 0;
    virtual void             PostInit() = 0;
    virtual void             Shutdown(void) = 0;
    virtual void             LevelInitPreEntity(char const* pMapName) = 0;
    virtual void             LevelInitPostEntity() = 0;
    virtual void             LevelShutdown(void) = 0;
    virtual ClientClass*     GetAllClasses(void) = 0;

    PushVirtual( DispatchUserMessage( int MessageType, int a1, int a2, LPVOID Data ), 38, bool( __thiscall* )( LPVOID, int, int, int, LPVOID ), MessageType, a1, a2, Data );
};