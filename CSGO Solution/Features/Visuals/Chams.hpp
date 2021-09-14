#pragma ocne
#include "../SDK/Includes.hpp"
#include "../Settings.hpp"

class C_Chams
{
public:
	virtual void CreateMaterials( );
	virtual void DrawModel( C_ChamsSettings Settings, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull = false, bool bXQZ = false );
	virtual void OnModelRender( IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix );
	virtual void ApplyMaterial( int32_t iMaterial, bool bIgnoreZ, Color aColor, bool bCustom = false, bool bShotChams = false );
	virtual bool GetInterpolatedMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix );
private:
	C_Material* m_pFlatMaterial = nullptr;
	C_Material* m_pGlowMaterial = nullptr;
	C_Material* m_pPulseMaterial = nullptr;
	C_Material* m_pRegularMaterial = nullptr;
	C_Material* m_pGlassMaterial = nullptr;
	C_Material* m_pGhostMaterial = nullptr;
	C_Material* m_pHaloMaterial = nullptr;
};

inline C_Chams* g_Chams = new C_Chams( );