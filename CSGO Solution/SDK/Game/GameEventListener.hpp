#pragma once
#include "GameEvent.hpp"

class C_GameEventListener
{
public:
    virtual ~C_GameEventListener(void) {}

    virtual void FireGameEvent( C_GameEvent* m_pEvent) = 0;
    virtual int  GetEventDebugID(void) = 0;
public:
    int m_iDebugId;
};