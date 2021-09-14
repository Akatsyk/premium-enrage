#pragma once

#include "../Utils/IHandleEntity.hpp"
#include "IClientEntity.hpp"

class IClientNetworkable;
class C_ClientEntityList
{
public:
    virtual IClientNetworkable*   GetClientNetworkable(int entnum) = 0;
    virtual void*                 vtablepad0x1(void) = 0;
    virtual void*                 vtablepad0x2(void) = 0;
    virtual C_ClientEntity*       GetClientEntity(int entNum) = 0;
    virtual C_ClientEntity*       GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
    virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
    virtual int                   GetHighestEntityIndex(void) = 0;
    virtual void                  SetMaxEntities(int maxEnts) = 0;
    virtual int                   GetMaxEntities() = 0;
};