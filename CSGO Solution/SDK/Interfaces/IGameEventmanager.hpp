#pragma once

#include <cstdint>
#include "../Game/GameEventListener.hpp"

#define EVENT_DEBUG_ID_INIT 42 
#define EVENT_DEBUG_ID_SHUTDOWN 13  

class C_GameEventManager
{
public:
    virtual             ~C_GameEventManager( ) = 0;
    virtual int         LoadEventsFromFile(const char *filename) = 0;
    virtual void        Reset() = 0;
    virtual bool        AddListener(C_GameEventListener *listener, const char *name, bool bServerSide) = 0;
    virtual bool        FindListener(C_GameEventListener *listener, const char *name) = 0;
    virtual int         RemoveListener(C_GameEventListener *listener) = 0;
    virtual C_GameEvent*CreateEvent(const char *name, bool bForce, unsigned int dwUnknown) = 0;
    virtual bool        FireEvent(C_GameEvent *event, bool bDontBroadcast = false) = 0;
    virtual bool        FireEventClientSide(C_GameEvent *event) = 0;
    virtual C_GameEvent*DuplicateEvent(C_GameEvent *event) = 0;
    virtual void        FreeEvent(C_GameEvent *event) = 0;
    virtual bool        SerializeEvent(C_GameEvent *event, bf_write *buf) = 0;
    virtual C_GameEvent*UnserializeEvent(bf_read *buf) = 0;
};

class C_CustomEventListener : public C_GameEventListener
{
public:
    virtual void FireGameEvent( C_GameEvent* pEvent );
    virtual int  GetEventDebugID(void) { return 42; };
public:
    int m_iDebugId;
};