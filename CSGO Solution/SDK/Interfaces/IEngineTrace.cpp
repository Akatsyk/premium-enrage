#include <cstdint>
#include "../SDK/Includes.hpp"

void C_EngineTrace::TraceLine(const Vector& src, const Vector& dst, int mask, IHandleEntity* entity, int collision_group, CGameTrace* trace) {
    std::uintptr_t filter[4] = {
        *reinterpret_cast<std::uintptr_t*>(g_Globals.m_AddressList.m_TraceFilterSimple),
        reinterpret_cast<std::uintptr_t>(entity),
        ( uintptr_t )( collision_group ),
        0
    };

    auto ray = Ray_t();
    ray.Init(src, dst);

    return TraceRay( ray, mask, ( CTraceFilter* )(filter), trace );
}

void C_EngineTrace::TraceHull(const Vector& src, const Vector& dst, const Vector& mins, const Vector& maxs, int mask, IHandleEntity* entity, int collision_group, CGameTrace* trace) {
    std::uintptr_t filter[4] = {
        *reinterpret_cast<std::uintptr_t*>(g_Globals.m_AddressList.m_TraceFilterSimple),
        reinterpret_cast<std::uintptr_t>(entity),
         (uintptr_t)(collision_group),
        0
    };

    auto ray = Ray_t();
    ray.Init(src, dst, mins, maxs);

    return TraceRay( ray, mask, ( CTraceFilter* )(filter), trace );
}