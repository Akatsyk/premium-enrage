#pragma once
#include "../SDK/Includes.hpp"

struct C_ShotChamsData 
{
    int32_t m_iEntIndex;
    ModelRenderInfo_t m_RenderInfo;
    DrawModelState_t m_DrawState;
    float_t m_flTimeToDraw;
    
    std::array < matrix3x4_t, 128 > m_aMatrix;
    matrix3x4_t m_aWorldMatrix;
};

class C_ShotChams
{
public:
	virtual void OnDrawModel( );
	virtual void OnRageBotFire( C_BasePlayer* pPlayer );
    virtual void ClearData( ) { m_aShotChams.clear( ); };
private:
    std::vector < C_ShotChamsData > m_aShotChams;
};

inline C_ShotChams* g_ShotChams = new C_ShotChams( );