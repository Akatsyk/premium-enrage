#include "../Hooks.hpp"
#include "../Features/Visuals/Chams.hpp"

void __fastcall C_Hooks::hkDrawModelExecute_Model( LPVOID pEcx, uint32_t, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix )
{
	if ( g_Globals.m_Interfaces.m_ModelRender->IsForcedMaterialOverride( ) &&
		 !strstr( pInfo.pModel->szName, _S( "arms" ) ) &&
		 !strstr( pInfo.pModel->szName, _S( "weapons/v_" ) ) )
		 return g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model( pEcx, pCtx, pState, pInfo, aMatrix );

	return g_Chams->OnModelRender( pCtx, pState, pInfo, aMatrix );
}